#ifndef MANDELBROT_PTHREADS2_H
#define MANDELBROT_PTHREADS2_H

#include "common.h"

int mandelbrot_run_pthreads2(const MandelbrotParams *p, int *iterations, double *out_seconds);

#endif
