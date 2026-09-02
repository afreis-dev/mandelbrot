# Diário de Desenvolvimento — Mandelbrot

Preenchido **ao longo** do desenvolvimento (não retroativamente), mesmo padrão usado em `processflow` (Implementação 1). Serve de matéria-prima para quando o relatório for escrito por fora. Datas/horas são reais (saída de `date` no WSL), não estimadas.

Plano completo em `C:\Users\arthu\.claude\plans\c-users-arthu-downloads-implementa-o-2-effervescent-pond.md`.

## Estratégias

| Estratégia | Nome curto | Contexto |
|---|---|---|
| S1 | Núcleo comum único | `common.c` concentra pixel→complexo, iteração de escape e normalização; as 4 implementações chamam as mesmas funções, só o laço externo (divisão de trabalho) muda — garante as 4 imagens idênticas exigidas pelo enunciado |

## Diário de tentativas

| # | Estrat. | O que tentei | Resultado | Hipótese/Causa (se falhou) | Quando | Evidência |
|---|---|---|---|---|---|---|
| 1 | S1 | Fase 0: scaffold do projeto (`.gitignore`, `README.md`, `Makefile` com `wildcard src/*.c`, `src/main.c` placeholder que só retorna 0) + copiar os 3 casos oficiais do professor (`testes (1).tar`, na verdade um `.zip`) para `tests/oficiais/` | OK — `make clean && make` compilou sem warnings de primeira; `./mandelbrot` rodou com stdout vazio e exit code 0 | — | 02/09/2026 15:30 (WSL2 Ubuntu, gcc 15.2.0) | `make` e `./mandelbrot` executados dentro do WSL2, ver histórico de comandos da sessão |
| 2 | S1 | Antes de escrever qualquer código: validar empiricamente a fórmula de mapeamento pixel→complexo, o critério de escape e a normalização contra os 3 casos oficiais, testando 16 combinações plausíveis (centro-do-pixel vs borda-do-pixel vs endpoint, dois critérios de escape, 5 fórmulas de normalização) num script Python fora do repo | A primeira leva (pixel-centro + arredondamento, que era a suposição inicial do plano) não bateu com nenhum dos 3 casos. Ampliando a busca, "borda do pixel" (`px/largura`, sem `+0.5`) + critério de escape clássico + normalização por divisão inteira TRUNCADA (sem arredondar) bateu 112/112 valores nos 3 casos | A suposição inicial (centro do pixel + `round()`) era plausível mas errada — o professor usa a borda do pixel e trunca em vez de arredondar na normalização | 02/09/2026 (antes da Fase 0, durante o planejamento) | Script de verificação (fora do repo, `scratchpad/verify2.py`); plano atualizado com a fórmula correta antes de qualquer `.c` ser escrito |
