CC      := gcc
CFLAGS  := -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -O2 -g -Isrc -fopenmp -pthread
LDFLAGS := -fopenmp -pthread
SRC     := $(wildcard src/*.c)
OBJ     := $(SRC:.c=.o)

.PHONY: all clean test

all: mandelbrot

mandelbrot: $(OBJ)
	$(CC) $(CFLAGS) -o mandelbrot $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f mandelbrot $(OBJ) mandelbrot_afsr_*.pgm times.txt
	rm -f tests/out/*

test: all
	./mandelbrot 320 241 200 4
	@diff -q mandelbrot_afsr_serial.pgm mandelbrot_afsr_openmp.pgm    && echo "serial == openmp: OK"
	@diff -q mandelbrot_afsr_serial.pgm mandelbrot_afsr_pthreads1.pgm && echo "serial == pthreads1: OK"
	@diff -q mandelbrot_afsr_serial.pgm mandelbrot_afsr_pthreads2.pgm && echo "serial == pthreads2: OK"
	@cat times.txt
	@echo "--- casos oficiais (professor) ---"
	@bash tests/rodar_oficiais.sh ./mandelbrot
	@echo "--- casos invalidos: exit != 0 e stdout vazio ---"
	@bash tests/casos_invalidos.sh ./mandelbrot
