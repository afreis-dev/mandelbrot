CC      := gcc
CFLAGS  := -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -O2 -g -Isrc -fopenmp -pthread
LDFLAGS := -fopenmp -pthread
SRC     := $(wildcard src/*.c)
OBJ     := $(SRC:.c=.o)
BIN     := mandelbrot

.PHONY: all clean test

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(BIN) $(OBJ) mandelbrot_afsr_*.pgm times.txt
	rm -f tests/out/*

# Alvo "test" completo (diff entre implementacoes + casos oficiais + casos
# invalidos) entra na Fase 7, quando as 4 implementacoes existirem.
test: all
	@echo "make test: ainda incompleto (Fase 0) -- so garante que compila."
