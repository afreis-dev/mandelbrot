/*
 * mandelbrot - Implementacao 2 (Infraestrutura de Software, CESAR School)
 *
 * Fase 1: validacao completa dos argumentos da linha de comando
 * (mandelbrot_parse_args, em src/common.c). O calculo em si (as 4
 * implementacoes) ainda nao existe -- entra a partir da Fase 2/3.
 */

#include "common.h"

int main(int argc, char *argv[]) {
    MandelbrotParams params;

    if (mandelbrot_parse_args(argc, argv, &params) != 0) {
        return 1;
    }

    return 0;
}
