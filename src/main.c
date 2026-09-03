#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "openmp.h"
#include "pthreads1.h"
#include "pthreads2.h"
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

    if (mandelbrot_run_openmp(&params, iterations, &t_openmp) != 0) {
        free(iterations);
        return 1;
    }
    if (mandelbrot_write_pgm("mandelbrot_afsr_openmp.pgm", iterations, &params) != 0) {
        free(iterations);
        return 1;
    }

    if (mandelbrot_run_pthreads1(&params, iterations, &t_pthreads1) != 0) {
        free(iterations);
        return 1;
    }
    if (mandelbrot_write_pgm("mandelbrot_afsr_pthreads1.pgm", iterations, &params) != 0) {
        free(iterations);
        return 1;
    }

    if (mandelbrot_run_pthreads2(&params, iterations, &t_pthreads2) != 0) {
        free(iterations);
        return 1;
    }
    if (mandelbrot_write_pgm("mandelbrot_afsr_pthreads2.pgm", iterations, &params) != 0) {
        free(iterations);
        return 1;
    }

    if (mandelbrot_write_times("times.txt", t_serial, t_openmp, t_pthreads1, t_pthreads2) != 0) {
        free(iterations);
        return 1;
    }

    free(iterations);
    return 0;
}
