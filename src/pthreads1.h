#ifndef MANDELBROT_PTHREADS1_H
#define MANDELBROT_PTHREADS1_H

#include "common.h"

int mandelbrot_run_pthreads1(const MandelbrotParams *p, int *iterations, double *out_seconds);

#endif
