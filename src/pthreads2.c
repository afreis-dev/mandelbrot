#include "pthreads2.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Fila circular limitada de indices de linha, compartilhada entre a
 * thread despachante (produtora) e as threads trabalhadoras (consumidoras).
 * 'done' sinaliza "acabaram as linhas, pode sair quando a fila esvaziar".
 * 'abort' sinaliza "algo falhou na criacao das threads, todo mundo sai
 * ja" -- sem ela, uma falha de pthread_create no meio da criacao poderia
 * deixar o despachante ou trabalhadoras ja criadas bloqueadas pra sempre
 * esperando um sinal que nunca chegaria.
 */
typedef struct {
    int *buffer;
    int capacity;
    int head;
    int tail;
    int count;
    int done;
    int abort_flag;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} Pthreads2Queue;

typedef struct {
    Pthreads2Queue *queue;
    int height;
} Pthreads2DispatcherArgs;

typedef struct {
    Pthreads2Queue *queue;
    const MandelbrotParams *params;
    int *iterations;
} Pthreads2WorkerArgs;

static void *pthreads2_dispatcher(void *arg) {
    Pthreads2DispatcherArgs *d = (Pthreads2DispatcherArgs *) arg;
    Pthreads2Queue *q = d->queue;

    for (int row = 0; row < d->height; row++) {
        pthread_mutex_lock(&q->mutex);
        while (q->count == q->capacity && !q->abort_flag) {
            pthread_cond_wait(&q->not_full, &q->mutex);
        }
        if (q->abort_flag) {
            pthread_mutex_unlock(&q->mutex);
            return NULL;
        }
        q->buffer[q->tail] = row;
        q->tail = (q->tail + 1) % q->capacity;
        q->count++;
        pthread_cond_signal(&q->not_empty); /* so precisa acordar 1 consumidor */
        pthread_mutex_unlock(&q->mutex);
    }

    pthread_mutex_lock(&q->mutex);
    q->done = 1;
    pthread_cond_broadcast(&q->not_empty); /* acorda TODAS as trabalhadoras */
    pthread_mutex_unlock(&q->mutex);
    return NULL;
}

static void *pthreads2_worker(void *arg) {
    Pthreads2WorkerArgs *w = (Pthreads2WorkerArgs *) arg;
    Pthreads2Queue *q = w->queue;
    const MandelbrotParams *p = w->params;

    for (;;) {
        pthread_mutex_lock(&q->mutex);
        while (q->count == 0 && !q->done && !q->abort_flag) {
            pthread_cond_wait(&q->not_empty, &q->mutex);
        }
        if (q->abort_flag || (q->count == 0 && q->done)) {
            pthread_mutex_unlock(&q->mutex);
            break;
        }
        int row = q->buffer[q->head];
        q->head = (q->head + 1) % q->capacity;
        q->count--;
        pthread_cond_signal(&q->not_full);
        pthread_mutex_unlock(&q->mutex);

        /* calculo da linha inteira FORA do lock -- so a retirada do
         * indice e protegida, o trabalho de verdade roda livre. */
        for (int col = 0; col < p->width; col++) {
            double re, im;
            mandelbrot_pixel_to_complex(col, row, p->width, p->height, &re, &im);
            w->iterations[mandelbrot_index(row, col, p->width)] =
                mandelbrot_escape_iterations(re, im, p->max_iter);
        }
    }
    return NULL;
}

/* Libera tudo que foi inicializado/alocado ate o ponto da falha. Cada
 * ponteiro/flag pode estar em "ainda nao existe" e a funcao trata isso. */
static void pthreads2_cleanup(Pthreads2Queue *q, int mutex_ok, int not_empty_ok, int not_full_ok,
                               pthread_t *worker_tids, Pthreads2WorkerArgs *wargs) {
    if (not_full_ok) pthread_cond_destroy(&q->not_full);
    if (not_empty_ok) pthread_cond_destroy(&q->not_empty);
    if (mutex_ok) pthread_mutex_destroy(&q->mutex);
    free(q->buffer);
    free(worker_tids);
    free(wargs);
}

int mandelbrot_run_pthreads2(const MandelbrotParams *p, int *iterations, double *out_seconds) {
    int n = p->num_threads;

    /* A fila nunca precisa de mais espaco que o total de linhas -- esse
     * e o numero maximo de itens que o despachante vai empurrar. */
    int capacity = 2 * n;
    if (capacity > p->height) capacity = p->height;
    if (capacity < 1) capacity = 1;

    Pthreads2Queue queue;
    queue.buffer = malloc((size_t) capacity * sizeof(int));
    queue.capacity = capacity;
    queue.head = 0;
    queue.tail = 0;
    queue.count = 0;
    queue.done = 0;
    queue.abort_flag = 0;

    if (queue.buffer == NULL) {
        fprintf(stderr, "mandelbrot: pthreads2: falha ao alocar fila de trabalho\n");
        free(queue.buffer);
        return -1;
    }

    int mutex_ok = (pthread_mutex_init(&queue.mutex, NULL) == 0);
    int not_empty_ok = mutex_ok && (pthread_cond_init(&queue.not_empty, NULL) == 0);
    int not_full_ok = not_empty_ok && (pthread_cond_init(&queue.not_full, NULL) == 0);

    if (!mutex_ok || !not_empty_ok || !not_full_ok) {
        fprintf(stderr, "mandelbrot: pthreads2: falha ao inicializar mutex/condvar\n");
        pthreads2_cleanup(&queue, mutex_ok, not_empty_ok, not_full_ok, NULL, NULL);
        return -1;
    }

    pthread_t *worker_tids = malloc((size_t) n * sizeof(pthread_t));
    Pthreads2WorkerArgs *wargs = malloc((size_t) n * sizeof(Pthreads2WorkerArgs));
    if (worker_tids == NULL || wargs == NULL) {
        fprintf(stderr, "mandelbrot: pthreads2: falha ao alocar estruturas de threads\n");
        pthreads2_cleanup(&queue, mutex_ok, not_empty_ok, not_full_ok, worker_tids, wargs);
        return -1;
    }

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    Pthreads2DispatcherArgs dargs = { .queue = &queue, .height = p->height };
    pthread_t dispatcher_tid;
    int dispatcher_created = 0;
    int workers_created = 0;
    int failed = 0;

    int rc = pthread_create(&dispatcher_tid, NULL, pthreads2_dispatcher, &dargs);
    if (rc != 0) {
        fprintf(stderr, "mandelbrot: pthreads2: pthread_create (dispatcher): %s\n", strerror(rc));
        failed = 1;
    } else {
        dispatcher_created = 1;
    }

    for (int i = 0; !failed && i < n; i++) {
        wargs[i].queue = &queue;
        wargs[i].params = p;
        wargs[i].iterations = iterations;
        rc = pthread_create(&worker_tids[i], NULL, pthreads2_worker, &wargs[i]);
        if (rc != 0) {
            fprintf(stderr, "mandelbrot: pthreads2: pthread_create (worker %d): %s\n", i, strerror(rc));
            failed = 1;
            break;
        }
        workers_created++;
    }

    if (failed) {
        /* Sinaliza abort e acorda quem ja possa estar bloqueado esperando
         * (despachante esperando espaco, trabalhadoras esperando item) --
         * senao as threads ja criadas ficam presas pra sempre. */
        pthread_mutex_lock(&queue.mutex);
        queue.abort_flag = 1;
        pthread_cond_broadcast(&queue.not_full);
        pthread_cond_broadcast(&queue.not_empty);
        pthread_mutex_unlock(&queue.mutex);

        if (dispatcher_created) pthread_join(dispatcher_tid, NULL);
        for (int j = 0; j < workers_created; j++) pthread_join(worker_tids[j], NULL);

        pthreads2_cleanup(&queue, mutex_ok, not_empty_ok, not_full_ok, worker_tids, wargs);
        return -1;
    }

    pthread_join(dispatcher_tid, NULL);
    for (int i = 0; i < n; i++) {
        pthread_join(worker_tids[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    *out_seconds = (double) (t1.tv_sec - t0.tv_sec) + (double) (t1.tv_nsec - t0.tv_nsec) / 1e9;

    pthreads2_cleanup(&queue, mutex_ok, not_empty_ok, not_full_ok, worker_tids, wargs);
    return 0;
}
