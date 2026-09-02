#ifndef MANDELBROT_OPENMP_H
#define MANDELBROT_OPENMP_H

#include "common.h"

/*
 * Implementacao OpenMP: mesma matematica de common.c, laco externo (sobre
 * as linhas da imagem) paralelizado com "#pragma omp parallel for
 * schedule(static)" -- divisao estatica em blocos, o equivalente
 * conceitual da divisao por blocos do Pthreads1. Preenche *out_seconds
 * (medido com omp_get_wtime, so o calculo). Retorna 0.
 */
int mandelbrot_run_openmp(const MandelbrotParams *p, int *iterations, double *out_seconds);

#endif
