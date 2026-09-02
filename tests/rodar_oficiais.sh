#!/bin/bash
# tests/rodar_oficiais.sh <caminho-do-binario>
#
# Roda cada tests/oficiais/teste*.txt (enviados pelo professor) e compara
# a saida real com o bloco "Conteudo esperado:". Os proprios arquivos do
# professor usam rotulos ligeiramente diferentes entre si ("Comando:" vs
# "Comando de entrada:"; "Arquivo a verificar:" vs "Arquivos a
# verificar:") -- o parser casa por prefixo, nao por string exata. O nome
# do arquivo nos .txt usa "login" generico; trocamos por "afsr".
set -u
BIN="${1:-./mandelbrot}"
FAIL=0

for f in tests/oficiais/teste*.txt; do
    cmd=$(awk '/^Comando/{getline; print; exit}' "$f")
    arquivo=$(awk '/^Arquivo/{getline; print; exit}' "$f")
    arquivo=${arquivo/login/afsr}
    esperado=$(awk '/^Conteudo esperado:/{flag=1; next} flag' "$f")

    cmd_ajustado=${cmd/.\/mandelbrot/$BIN}
    eval "$cmd_ajustado" > /tmp/rodar_oficiais_stdout 2> /tmp/rodar_oficiais_stderr
    code=$?
    stdout_content=$(cat /tmp/rodar_oficiais_stdout)

    obtido=$(cat "$arquivo" 2>/dev/null)

    if [ "$code" -ne 0 ]; then
        echo "FALHOU [$f]: exit code $code (esperado 0) -- $(cat /tmp/rodar_oficiais_stderr)"
        FAIL=1
    elif [ -n "$stdout_content" ]; then
        echo "FALHOU [$f]: stdout deveria ser vazio, veio: '$stdout_content'"
        FAIL=1
    elif [ "$obtido" != "$esperado" ]; then
        echo "FALHOU [$f] ($arquivo): conteudo diferente do esperado"
        diff <(echo "$esperado") <(echo "$obtido")
        FAIL=1
    else
        echo "OK [$f] ($arquivo)"
    fi
done

rm -f /tmp/rodar_oficiais_stdout /tmp/rodar_oficiais_stderr
exit $FAIL
