#include "openmp.h"

#include <omp.h>

int mandelbrot_run_openmp(const MandelbrotParams *p, int *iterations, double *out_seconds) {
    double t0 = omp_get_wtime();

#pragma omp parallel for schedule(static) num_threads(p->num_threads)
    for (int row = 0; row < p->height; row++) {
        for (int col = 0; col < p->width; col++) {
            double re, im;
            mandelbrot_pixel_to_complex(col, row, p->width, p->height, &re, &im);
            iterations[mandelbrot_index(row, col, p->width)] =
                mandelbrot_escape_iterations(re, im, p->max_iter);
        }
    }

    double t1 = omp_get_wtime();
    *out_seconds = t1 - t0;
    return 0;
}
