#include "pthreads1.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    const MandelbrotParams *params;
    int *iterations;
    int start_row; /* inclusive */
    int end_row;   /* exclusivo */
} Pthreads1WorkerArgs;

static void *pthreads1_worker(void *arg) {
    Pthreads1WorkerArgs *w = (Pthreads1WorkerArgs *) arg;
    const MandelbrotParams *p = w->params;

    for (int row = w->start_row; row < w->end_row; row++) {
        for (int col = 0; col < p->width; col++) {
            double re, im;
            mandelbrot_pixel_to_complex(col, row, p->width, p->height, &re, &im);
            w->iterations[mandelbrot_index(row, col, p->width)] =
                mandelbrot_escape_iterations(re, im, p->max_iter);
        }
    }
    return NULL;
}

int mandelbrot_run_pthreads1(const MandelbrotParams *p, int *iterations, double *out_seconds) {
    int n = p->num_threads;

    pthread_t *tids = malloc((size_t) n * sizeof(pthread_t));
    Pthreads1WorkerArgs *wargs = malloc((size_t) n * sizeof(Pthreads1WorkerArgs));
    if (tids == NULL || wargs == NULL) {
        fprintf(stderr, "mandelbrot: pthreads1: falha ao alocar estruturas de threads\n");
        free(tids);
        free(wargs);
        return -1;
    }

    /* base+resto: as primeiras 'resto' threads recebem 1 linha a mais.
     * Essa formula sozinha ja cobre num_threads > altura sem 'if'
     * especial: threads excedentes recebem start==end (0 linhas). */
    int base = p->height / n;
    int remainder = p->height % n;

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    int created = 0;
    for (int i = 0; i < n; i++) {
        int rows = base + (i < remainder ? 1 : 0);
        int start = i * base + (i < remainder ? i : remainder);

        wargs[i].params = p;
        wargs[i].iterations = iterations;
        wargs[i].start_row = start;
        wargs[i].end_row = start + rows;

        int rc = pthread_create(&tids[i], NULL, pthreads1_worker, &wargs[i]);
        if (rc != 0) {
            /* pthread_create devolve o erro diretamente -- nao seta errno. */
            fprintf(stderr, "mandelbrot: pthreads1: pthread_create: %s\n", strerror(rc));
            for (int j = 0; j < created; j++) {
                pthread_join(tids[j], NULL);
            }
            free(tids);
            free(wargs);
            return -1;
        }
        created++;
    }

    for (int i = 0; i < n; i++) {
        pthread_join(tids[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    *out_seconds = (double) (t1.tv_sec - t0.tv_sec) + (double) (t1.tv_nsec - t0.tv_nsec) / 1e9;

    free(tids);
    free(wargs);
    return 0;
}
