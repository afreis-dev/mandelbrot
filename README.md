# Mandelbrot

Gerador do conjunto de Mandelbrot da disciplina de Infraestrutura de Software (Implementação 2). Uma única execução calcula a imagem quatro vezes — serial, OpenMP e duas estratégias de divisão de trabalho com Pthreads — e grava o tempo de cada uma em `times.txt`.

## Compilar

Requer `gcc` e `make` (Linux, Unix, macOS ou WSL).

    make

Limpar:

    make clean

## Executar

    ./mandelbrot <largura> <altura> <max_iteracoes> <num_threads>

Exemplo:

    ./mandelbrot 800 600 1000 4

Gera `mandelbrot_afsr_serial.pgm`, `mandelbrot_afsr_openmp.pgm`, `mandelbrot_afsr_pthreads1.pgm` e `mandelbrot_afsr_pthreads2.pgm` (as quatro com o mesmo conteúdo) e `times.txt`.

## Testar

    make test

Roda o binário, confere que as 4 saídas são idênticas entre si, valida contra os 3 casos oficiais em `tests/oficiais/` e roda a bateria de argumentos inválidos em `tests/casos_invalidos.sh`.

## Arquivos

| Arquivo | Responsabilidade |
|---|---|
| `src/main.c` | Orquestra as 4 implementações, timing, escrita dos arquivos de saída |
| `src/common.c` | Pixel→complexo, iteração de escape, normalização, escrita do `.pgm`, parsing/validação de argumentos |
| `src/serial.c` | Implementação serial |
| `src/openmp.c` | Implementação OpenMP (`schedule(static)`) |
| `src/pthreads1.c` | Pthreads — divisão estática em blocos fixos de linhas |
| `src/pthreads2.c` | Pthreads — dispatcher dinâmico (fila de trabalho limitada) |

## Ambiente

Desenvolvido e testado no WSL2 (Ubuntu), Windows 11.

## Autor

afsr@cesar.school
