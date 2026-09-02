#ifndef MANDELBROT_SERIAL_H
#define MANDELBROT_SERIAL_H

#include "common.h"

/*
 * Implementacao serial: calcula iterations[0..width*height) em ordem, sem
 * paralelismo, usando as mesmas funcoes nucleo de common.c que as outras
 * 3 implementacoes (garante imagem identica). Preenche *out_seconds com o
 * tempo do calculo em si (nao inclui alocacao do buffer nem escrita do
 * arquivo). Retorna 0 em sucesso; a assinatura devolve int (nao void) pra
 * ficar simetrica as implementacoes com threads, mesmo que este caminho
 * nunca falhe de verdade.
 */
int mandelbrot_run_serial(const MandelbrotParams *p, int *iterations, double *out_seconds);

#endif
