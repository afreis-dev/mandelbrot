/*
 * mandelbrot - Implementacao 2 (Infraestrutura de Software, CESAR School)
 *
 * Orquestra as 4 implementacoes (serial, OpenMP, Pthreads1, Pthreads2)
 * sobre um unico buffer de iteracoes reaproveitado entre elas, escrevendo
 * um .pgm por implementacao e um times.txt com as 4 medicoes.
 *
 * Fase 3: so a serial existe de verdade. OpenMP/Pthreads1/Pthreads2 entram
 * nas Fases 4, 5 e 6 -- ate la, times.txt mostra 0.000000s pra elas.
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "serial.h"

int main(int argc, char *argv[]) {
    MandelbrotParams params;
    if (mandelbrot_parse_args(argc, argv, &params) != 0) {
        return 1;
    }

    size_t pixel_count;
    if (mandelbrot_buffer_pixel_count(params.width, params.height, &pixel_count) != 0) {
        return 1;
    }

    int *iterations = malloc(pixel_count * sizeof(int));
    if (iterations == NULL) {
        fprintf(stderr, "mandelbrot: falha ao alocar buffer de iteracoes\n");
        return 1;
    }

    double t_serial = 0.0, t_openmp = 0.0, t_pthreads1 = 0.0, t_pthreads2 = 0.0;

    if (mandelbrot_run_serial(&params, iterations, &t_serial) != 0) {
        free(iterations);
        return 1;
    }
    if (mandelbrot_write_pgm("mandelbrot_afsr_serial.pgm", iterations, &params) != 0) {
        free(iterations);
        return 1;
    }

    /* TODO Fase 4: mandelbrot_run_openmp + mandelbrot_afsr_openmp.pgm */
    /* TODO Fase 5: mandelbrot_run_pthreads1 + mandelbrot_afsr_pthreads1.pgm */
    /* TODO Fase 6: mandelbrot_run_pthreads2 + mandelbrot_afsr_pthreads2.pgm */

    if (mandelbrot_write_times("times.txt", t_serial, t_openmp, t_pthreads1, t_pthreads2) != 0) {
        free(iterations);
        return 1;
    }

    free(iterations);
    return 0;
}
