#include "common.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static void print_usage(void) {
    fprintf(stderr,
            "uso: mandelbrot <largura> <altura> <max_iteracoes> <num_threads>\n");
}

/*
 * Converte 'text' para um inteiro estritamente positivo, cabendo em int.
 * Rejeita: string vazia, lixo apos o numero, overflow de 'long' (ERANGE)
 * ou overflow ao encaixar em 'int', e valores <= 0. Em qualquer erro, ja
 * imprime a mensagem em stderr (usando 'field_name' no texto) e retorna
 * -1 -- quem chama nao precisa imprimir nada de novo.
 */
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
