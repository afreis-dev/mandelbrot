#ifndef MANDELBROT_SERIAL_H
#define MANDELBROT_SERIAL_H

#include "common.h"

int mandelbrot_run_serial(const MandelbrotParams *p, int *iterations, double *out_seconds);

#endif
