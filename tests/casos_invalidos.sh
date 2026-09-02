#!/bin/bash
# tests/casos_invalidos.sh <caminho-do-binario>
#
# Roda uma bateria de invocacoes invalidas e confere, pra cada uma: stdout
# vazio, exit code != 0, e alguma mensagem em stderr.
#
# Nota sobre falha de pthread_create (Fase 7): forcada e verificada de
# verdade durante o desenvolvimento (ver docs/diario.md, entradas 14-16),
# mas NAO fica automatizada aqui contra o binario completo: como as 4
# implementacoes rodam em sequencia numa unica execucao compartilhando o
# mesmo num_threads, um limite de recursos apertado o suficiente pra
# quebrar pthread_create tambem quebra a criacao de threads do OpenMP
# (que roda primeiro) -- e o libgomp aborta o processo com a MENSAGEM DELE
# (nao a minha) antes do meu codigo em pthreads1.c/pthreads2.c rodar. A
# falha de pthread_create do meu proprio codigo foi verificada em
# isolamento com um harness a parte (fora deste repo); o comportamento em
# si (sem crash, sem trava, mensagem coerente, limpeza correta mesmo com
# threads parcialmente criadas) esta confirmado e documentado no diario.
set -u
BIN="${1:-./mandelbrot}"
FAIL=0
TMP_ERR=$(mktemp)

check_invalid() {
    local desc="$1"; shift
    local out code err
    out=$("$BIN" "$@" 2>"$TMP_ERR")
    code=$?
    err=$(cat "$TMP_ERR")

    if [ -n "$out" ]; then
        echo "FALHOU [$desc]: stdout deveria ser vazio, veio: '$out'"
        FAIL=1
    elif [ "$code" -eq 0 ]; then
        echo "FALHOU [$desc]: exit code deveria ser != 0, veio 0"
        FAIL=1
    elif [ -z "$err" ]; then
        echo "FALHOU [$desc]: stderr deveria conter mensagem de erro, veio vazio"
        FAIL=1
    else
        echo "OK [$desc] (exit=$code; stderr: $err)"
    fi
}

# --- argc incorreto ---
check_invalid "sem argumentos"
check_invalid "1 argumento" 10
check_invalid "2 argumentos" 10 10
check_invalid "3 argumentos" 10 10 10
check_invalid "5 argumentos (excesso)" 10 10 10 4 99

# --- campos nao-numericos / lixo a direita ---
check_invalid "largura nao-numerica" abc 10 10 4
check_invalid "altura com lixo a direita" 10 10abc 10 4
check_invalid "max_iteracoes vazio" 10 10 "" 4
check_invalid "num_threads com ponto decimal" 10 10 10 3.5

# --- zero / negativo ---
check_invalid "largura zero" 0 10 10 4
check_invalid "altura negativa" 10 -5 10 4
check_invalid "max_iteracoes zero" 10 10 0 4
check_invalid "num_threads negativo" 10 10 10 -1

# --- overflow na conversao ---
check_invalid "largura gigante (overflow de long)" 99999999999999999999 10 10 4

echo ""
echo "--- falha real de malloc (ulimit -v baixo + imagem grande demais) ---"
OUT=$(ulimit -v 32768; "$BIN" 20000 20000 10 1 2>"$TMP_ERR")
CODE=$?
ERR=$(cat "$TMP_ERR")
if [ -n "$OUT" ]; then
    echo "FALHOU [malloc sob ulimit -v baixo]: stdout deveria ser vazio, veio: '$OUT'"
    FAIL=1
elif [ "$CODE" -eq 0 ]; then
    echo "FALHOU [malloc sob ulimit -v baixo]: exit code deveria ser != 0, veio 0"
    FAIL=1
elif [ -z "$ERR" ]; then
    echo "FALHOU [malloc sob ulimit -v baixo]: stderr deveria conter mensagem de erro, veio vazio"
    FAIL=1
else
    echo "OK [malloc sob ulimit -v baixo] (exit=$CODE; stderr: $ERR)"
fi

rm -f "$TMP_ERR"
exit $FAIL
