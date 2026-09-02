#ifndef MANDELBROT_COMMON_H
#define MANDELBROT_COMMON_H

typedef struct {
    int width;
    int height;
    int max_iter;
    int num_threads;
} MandelbrotParams;

/*
 * Valida argc/argv e preenche *out. Retorna 0 em sucesso, -1 em erro
 * (mensagem coerente ja impressa em stderr; nada e impresso em stdout).
 *
 * Espera exatamente 4 argumentos (alem do nome do programa): largura,
 * altura, max_iteracoes e num_threads, todos inteiros estritamente
 * positivos.
 */
int mandelbrot_parse_args(int argc, char *argv[], MandelbrotParams *out);

#endif
