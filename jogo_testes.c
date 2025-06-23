#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep

#include "jogo.h"

int main ()
{
    struct tabuleiro_t *tabuleiro = cria_tabuleiro();
    struct jogador_t *jogador = cria_jogador();
    unsigned int tesouro;
    unsigned int movimentacao;

    limpa_terminal();
    tesouro = encontrou_tesouro(tabuleiro, jogador);
    imprime_tabuleiro(tabuleiro, jogador);
    
    int movimento = 0;
    unsigned int procurando = 1;
    while (procurando)
    {
        tesouro = encontrou_tesouro(tabuleiro, jogador);
        movimento = le_movimento();
        if (movimento)
            movimentacao = movimenta_jogador(tabuleiro, jogador, movimento);
        else
            printf("Tecla inválida! Pressione uma das setas!\n");
        if (movimentacao)
        {
            //Limpa o terminal e imprime novamente
            limpa_terminal();
            tesouro = encontrou_tesouro(tabuleiro, jogador);
            imprime_tabuleiro(tabuleiro, jogador);
        }
        else
            printf("Bateu na parede! 🤕\n");

        procurando = procurando_tesouros(jogador);
    }

    //Só para sair de cima da medalha
    movimento = le_movimento();
    if (movimento)
        movimentacao = movimenta_jogador(tabuleiro, jogador, movimento);
    else
        printf("Tecla inválida! Pressione uma das setas!\n");
    if (movimentacao)
    {
        //Limpa o terminal e imprime novamente
        limpa_terminal();
        tesouro = encontrou_tesouro(tabuleiro, jogador);
        imprime_tabuleiro(tabuleiro, jogador);
    }
    else
        printf("Bateu na parede! 🤕\n");

    if (!procurando)
        mensagem_vitoria(jogador);

    infos_jogo(tabuleiro, jogador);
    
     destroi_tabuleiro(tabuleiro, jogador);
}