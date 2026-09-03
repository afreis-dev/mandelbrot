#include "common.h"

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(void) {
    fprintf(stderr,
            "uso: mandelbrot <largura> <altura> <max_iteracoes> <num_threads>\n");
}

static int parse_positive_int(const char *text, const char *field_name, int *out) {
    if (text == NULL || text[0] == '\0') {
        fprintf(stderr, "mandelbrot: valor invalido para %s: ''\n", field_name);
        return -1;
    }

    errno = 0;
    char *endptr = NULL;
    long value = strtol(text, &endptr, 10);

    if (endptr == text || *endptr != '\0') {
        fprintf(stderr, "mandelbrot: valor invalido para %s: '%s'\n", field_name, text);
        return -1;
    }
    if (errno == ERANGE || value > INT_MAX || value < INT_MIN) {
        fprintf(stderr, "mandelbrot: valor invalido para %s (fora da faixa representavel): '%s'\n",
                field_name, text);
        return -1;
    }
    if (value <= 0) {
        fprintf(stderr, "mandelbrot: valor invalido para %s (deve ser maior que zero): '%s'\n",
                field_name, text);
        return -1;
    }

    *out = (int) value;
    return 0;
}

int mandelbrot_parse_args(int argc, char *argv[], MandelbrotParams *out) {
    if (argc != 5) {
        fprintf(stderr, "mandelbrot: numero incorreto de argumentos\n");
        print_usage();
        return -1;
    }

    if (parse_positive_int(argv[1], "largura", &out->width) != 0) return -1;
    if (parse_positive_int(argv[2], "altura", &out->height) != 0) return -1;
    if (parse_positive_int(argv[3], "max_iteracoes", &out->max_iter) != 0) return -1;
    if (parse_positive_int(argv[4], "num_threads", &out->num_threads) != 0) return -1;

    return 0;
}

int mandelbrot_buffer_pixel_count(int width, int height, size_t *out) {
    size_t w = (size_t) width;
    size_t h = (size_t) height;

    if (w != 0 && h > SIZE_MAX / w) {
        fprintf(stderr, "mandelbrot: dimensoes de imagem grandes demais (largura*altura estoura)\n");
        return -1;
    }
    size_t count = w * h;

    if (count != 0 && sizeof(int) > SIZE_MAX / count) {
        fprintf(stderr, "mandelbrot: dimensoes de imagem grandes demais (buffer estoura)\n");
        return -1;
    }

    *out = count;
    return 0;
}

void mandelbrot_pixel_to_complex(int px, int py, int width, int height, double *re, double *im) {
    *re = MANDELBROT_RE_MIN + ((double) px / (double) width) * (MANDELBROT_RE_MAX - MANDELBROT_RE_MIN);
    *im = MANDELBROT_IM_MAX - ((double) py / (double) height) * (MANDELBROT_IM_MAX - MANDELBROT_IM_MIN);
}

int mandelbrot_escape_iterations(double c_re, double c_im, int max_iter) {
    double zr = 0.0, zi = 0.0;
    int count = 0;

    while (count < max_iter && (zr * zr + zi * zi) <= 4.0) {
        double next_zr = zr * zr - zi * zi + c_re;
        double next_zi = 2.0 * zr * zi + c_im;
        zr = next_zr;
        zi = next_zi;
        count++;
    }

    return count;
}

int mandelbrot_write_pgm(const char *path, const int *iterations, const MandelbrotParams *p) {
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        fprintf(stderr, "mandelbrot: falha ao criar arquivo de saida '%s': %s\n", path, strerror(errno));
        return -1;
    }

    int write_failed = 0;
    for (int row = 0; row < p->height && !write_failed; row++) {
        if (row > 0 && fputc('\n', f) == EOF) {
            write_failed = 1;
            break;
        }
        for (int col = 0; col < p->width; col++) {
            size_t idx = mandelbrot_index(row, col, p->width);
            int intensity = (int) (((long long) iterations[idx] * 255) / p->max_iter);
            if (fprintf(f, col == 0 ? "%d" : " %d", intensity) < 0) {
                write_failed = 1;
                break;
            }
        }
    }

    if (fclose(f) != 0 || write_failed) {
        fprintf(stderr, "mandelbrot: falha ao escrever arquivo de saida '%s': %s\n", path, strerror(errno));
        return -1;
    }
    return 0;
}

int mandelbrot_write_times(const char *path, double t_serial, double t_openmp,
                            double t_pthreads1, double t_pthreads2) {
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        fprintf(stderr, "mandelbrot: falha ao criar '%s': %s\n", path, strerror(errno));
        return -1;
    }

    int ok = fprintf(f, "Serial: %.6fs\nOpenMP: %.6fs\nPthreads1: %.6fs\nPthreads2: %.6fs\n",
                      t_serial, t_openmp, t_pthreads1, t_pthreads2) >= 0;

    if (fclose(f) != 0 || !ok) {
        fprintf(stderr, "mandelbrot: falha ao escrever '%s': %s\n", path, strerror(errno));
        return -1;
    }
    return 0;
}
