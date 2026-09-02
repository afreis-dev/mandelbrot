#include "serial.h"

#include <time.h>

int mandelbrot_run_serial(const MandelbrotParams *p, int *iterations, double *out_seconds) {
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    for (int row = 0; row < p->height; row++) {
        for (int col = 0; col < p->width; col++) {
            double re, im;
            mandelbrot_pixel_to_complex(col, row, p->width, p->height, &re, &im);
            iterations[mandelbrot_index(row, col, p->width)] =
                mandelbrot_escape_iterations(re, im, p->max_iter);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    *out_seconds = (double) (t1.tv_sec - t0.tv_sec) + (double) (t1.tv_nsec - t0.tv_nsec) / 1e9;
    return 0;
}
