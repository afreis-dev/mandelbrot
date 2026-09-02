#ifndef MANDELBROT_PTHREADS1_H
#define MANDELBROT_PTHREADS1_H

#include "common.h"

/*
 * Implementacao Pthreads com divisao ESTATICA em blocos fixos de linhas:
 * cria sempre exatamente p->num_threads threads, cada uma com um
 * intervalo [start, end) de linhas calculado ANTES de qualquer
 * pthread_create -- sem coordenacao em tempo de execucao, sem mutex
 * (regioes de escrita no buffer sao disjuntas por construcao). E o
 * equivalente conceitual do schedule(static) do OpenMP. Retorna 0/-1
 * (erro na criacao das threads, ja com stderr preenchido).
 */
int mandelbrot_run_pthreads1(const MandelbrotParams *p, int *iterations, double *out_seconds);

#endif
