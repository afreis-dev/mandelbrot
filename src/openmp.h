#ifndef MANDELBROT_OPENMP_H
#define MANDELBROT_OPENMP_H

#include "common.h"

int mandelbrot_run_openmp(const MandelbrotParams *p, int *iterations, double *out_seconds);

#endif
