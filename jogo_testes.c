#include <stdio.h>
#include <stdlib.h>

#include "jogo.h"

int main ()
{
    struct tabuleiro_t *tabuleiro = cria_tabuleiro();
    struct jogador_t *jogador = cria_jogador();

    imprime_tabuleiro(tabuleiro, jogador);

    destroi_tabuleiro(tabuleiro, jogador);
}