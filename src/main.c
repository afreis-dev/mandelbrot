/*
 * mandelbrot - Implementacao 2 (Infraestrutura de Software, CESAR School)
 *
 * Fase 2: laco serial PROVISORIO, so para validar as funcoes nucleo de
 * common.c (pixel->complexo, escape, normalizacao, escrita do .pgm)
 * contra o caso oficial teste1_serial.txt antes de estruturar o projeto
 * de verdade. Este laco e extraido para src/serial.c na Fase 3, junto
 * com a orquestracao real das 4 implementacoes e o times.txt.
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

int main(int argc, char *argv[]) {
    MandelbrotParams params;
    if (mandelbrot_parse_args(argc, argv, &params) != 0) {
        return 1;
    }

    size_t pixel_count;
    if (mandelbrot_buffer_pixel_count(params.width, params.height, &pixel_count) != 0) {
        return 1;
    }

    int *iterations = malloc(pixel_count * sizeof(int));
    if (iterations == NULL) {
        fprintf(stderr, "mandelbrot: falha ao alocar buffer de iteracoes\n");
        return 1;
    }

    for (int row = 0; row < params.height; row++) {
        for (int col = 0; col < params.width; col++) {
            double re, im;
            mandelbrot_pixel_to_complex(col, row, params.width, params.height, &re, &im);
            iterations[mandelbrot_index(row, col, params.width)] =
                mandelbrot_escape_iterations(re, im, params.max_iter);
        }
    }

    int status = mandelbrot_write_pgm("mandelbrot_afsr_serial.pgm", iterations, &params);

    free(iterations);
    return status == 0 ? 0 : 1;
}
