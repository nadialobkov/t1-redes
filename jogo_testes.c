#include <stdio.h>
#include <stdlib.h>

#include "jogo.h"

int main ()
{
    struct tabuleiro_t *tabuleiro = cria_tabuleiro();
    struct jogador_t *jogador = cria_jogador();
    unsigned int tesouro;
    unsigned int movimentacao;

    tesouro = encontrou_tesouro(tabuleiro, jogador);
    imprime_tabuleiro(tabuleiro, jogador);

    printf("------------------------------------------------------------------------------\n");

    //Cima
    movimentacao = movimenta_jogador(tabuleiro, jogador, 11);
    tesouro = encontrou_tesouro(tabuleiro, jogador);
    imprime_tabuleiro(tabuleiro, jogador);

    printf("------------------------------------------------------------------------------\n");
    printf("vou movimentar a jogadora\n");
    //Direita
    movimentacao = movimenta_jogador(tabuleiro, jogador, 10);
    tesouro = encontrou_tesouro(tabuleiro, jogador);
    imprime_tabuleiro(tabuleiro, jogador);

    printf("------------------------------------------------------------------------------\n");
    //Baixo
    movimentacao = movimenta_jogador(tabuleiro, jogador, 12);
    tesouro = encontrou_tesouro(tabuleiro, jogador);
    imprime_tabuleiro(tabuleiro, jogador);

    printf("------------------------------------------------------------------------------\n");

    //Esquerda
    movimentacao = movimenta_jogador(tabuleiro, jogador, 13);
    tesouro = encontrou_tesouro(tabuleiro, jogador);
    imprime_tabuleiro(tabuleiro, jogador);

    printf("------------------------------------------------------------------------------\n");

    //Movimentação inválida
    movimentacao = movimenta_jogador(tabuleiro, jogador, 13);
    tesouro = encontrou_tesouro(tabuleiro, jogador);
    imprime_tabuleiro(tabuleiro, jogador);
     if (!movimentacao)
         printf("Bateu na parede! 🤕\n");

     destroi_tabuleiro(tabuleiro, jogador);
}