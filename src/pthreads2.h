#ifndef MANDELBROT_PTHREADS2_H
#define MANDELBROT_PTHREADS2_H

#include "common.h"

/*
 * Implementacao Pthreads com estrategia de divisao DINAMICA, diferente da
 * divisao estatica do Pthreads1: uma thread "despachante" extra (NAO
 * conta como um dos p->num_threads, e NUNCA calcula nenhum pixel) empurra
 * indices de linha numa fila circular limitada; p->num_threads threads
 * "trabalhadoras" consomem indices da fila sob demanda e calculam a linha
 * inteira -- balanceamento de carga dinamico (padrao boss-worker).
 *
 * Retorna 0/-1 (erro na alocacao ou na criacao de threads/mutex/condvar,
 * ja com stderr preenchido e sem deixar nenhuma thread presa).
 */
int mandelbrot_run_pthreads2(const MandelbrotParams *p, int *iterations, double *out_seconds);

#endif
