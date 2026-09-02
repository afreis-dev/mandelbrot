#ifndef MANDELBROT_COMMON_H
#define MANDELBROT_COMMON_H

#include <stddef.h>

/* Regiao do plano complexo representada pela imagem (fixa pelo enunciado). */
#define MANDELBROT_RE_MIN (-2.0)
#define MANDELBROT_RE_MAX ( 1.0)
#define MANDELBROT_IM_MIN (-1.5)
#define MANDELBROT_IM_MAX ( 1.5)

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

/*
 * Calcula width*height em size_t, checando overflow tanto no produto em
 * si quanto no produto por sizeof(int) (tamanho real do buffer a alocar).
 * Retorna 0 e preenche *out em sucesso; -1 (com mensagem em stderr) se as
 * dimensoes forem grandes demais pra representar com seguranca.
 */
int mandelbrot_buffer_pixel_count(int width, int height, size_t *out);

/* Indice linear (row-major) de um pixel no buffer. Sempre em size_t --
 * nunca calcular "na mao" em int, que estoura bem antes. */
static inline size_t mandelbrot_index(int row, int col, int width) {
    return (size_t) row * (size_t) width + (size_t) col;
}

/*
 * Mapeia o pixel (px, py) de uma imagem width x height para o ponto do
 * plano complexo correspondente, usando a BORDA do pixel (nao o centro):
 * re = RE_MIN + (px/width)*(RE_MAX-RE_MIN); im = IM_MAX - (py/height)*(IM_MAX-IM_MIN).
 * Formula validada empiricamente contra os 3 casos oficiais do professor
 * (ver docs/diario.md) -- nao mudar sem revalidar contra tests/oficiais/.
 */
void mandelbrot_pixel_to_complex(int px, int py, int width, int height, double *re, double *im);

/*
 * Nucleo do calculo: z0=0, aplica z=z^2+c ate |z|^2>4 (escapou) ou ate
 * max_iter. Retorna o numero de iteracoes realizadas, em [0, max_iter].
 *
 * Funcao comum de verdade (nao "static inline"), compilada uma unica vez
 * e chamada por todas as 4 implementacoes -- e o que garante que as 4
 * imagens saiam identicas (ver docs/diario.md / plano, secao "Modulo comum").
 */
int mandelbrot_escape_iterations(double c_re, double c_im, int max_iter);

/*
 * Escreve o arquivo de saida no formato literal exigido pelo enunciado:
 * SEM cabecalho de formato, um valor de intensidade por pixel (0-255,
 * normalizado por divisao inteira truncada), separados por espaco, uma
 * linha de texto por linha da imagem. 'iterations' tem width*height
 * elementos (indexados via mandelbrot_index). Retorna 0/-1.
 */
int mandelbrot_write_pgm(const char *path, const int *iterations, const MandelbrotParams *p);

/* Escreve times.txt com o tempo (em segundos) de cada uma das 4
 * implementacoes, no formato "Label: 0.000000s". Retorna 0/-1. */
int mandelbrot_write_times(const char *path, double t_serial, double t_openmp,
                            double t_pthreads1, double t_pthreads2);

#endif
