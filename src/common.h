#ifndef MANDELBROT_COMMON_H
#define MANDELBROT_COMMON_H

#include <stddef.h>

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

int mandelbrot_parse_args(int argc, char *argv[], MandelbrotParams *out);

int mandelbrot_buffer_pixel_count(int width, int height, size_t *out);

static inline size_t mandelbrot_index(int row, int col, int width) {
    return (size_t) row * (size_t) width + (size_t) col;
}

void mandelbrot_pixel_to_complex(int px, int py, int width, int height, double *re, double *im);

int mandelbrot_escape_iterations(double c_re, double c_im, int max_iter);

int mandelbrot_write_pgm(const char *path, const int *iterations, const MandelbrotParams *p);

int mandelbrot_write_times(const char *path, double t_serial, double t_openmp,
                            double t_pthreads1, double t_pthreads2);

#endif
