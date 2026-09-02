#!/bin/bash
# tests/casos_invalidos.sh <caminho-do-binario>
#
# Roda uma bateria de invocacoes invalidas e confere, pra cada uma: stdout
# vazio, exit code != 0, e alguma mensagem em stderr. Fase 1: so os casos
# de parsing de argumentos. Os casos de falha de malloc/pthread_create
# (que precisam das implementacoes com threads) entram na Fase 7.
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

rm -f "$TMP_ERR"
exit $FAIL
