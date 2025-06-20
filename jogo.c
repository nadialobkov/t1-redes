#include <stdlib.h>
#include <stdio.h>
//#include <time.h>         //depois de pronto, para ter tesouros em posições diferentes a cada jogada
#include "jogo.h"
 
//Implementações

struct tabuleiro_t* cria_tabuleiro()
{
    struct tabuleiro_t *tabuleiro = malloc(sizeof(struct tabuleiro_t));
    if (!tabuleiro)
    {
        printf("Erro ao criar o Tabuleiro.\n");
        return NULL;
    }

    //Inicializações
    for (unsigned int i = 0; i < 8; i++)
    {
        tabuleiro->posicao_tesouro[i].x = 0;
        tabuleiro->posicao_tesouro[i].y = 0;

        for (unsigned int j = 0; j < 8; j++)
            tabuleiro->posicoes[i][j] = NAO_VISITADA;
    }

    //Atribuindo os arquivos
    tabuleiro->tesouros[0] = "tesouros/0.jpg";
    tabuleiro->tesouros[1] = "tesouros/1.txt";
    tabuleiro->tesouros[2] = "tesouros/2.txt";
    tabuleiro->tesouros[3] = "tesouros/3.txt";
    tabuleiro->tesouros[4] = "tesouros/4.txt";
    tabuleiro->tesouros[5] = "tesouros/5.mp4";
    tabuleiro->tesouros[6] = "tesouros/6.txt";
    tabuleiro->tesouros[7] = "tesouros/7.jpg";

    //Gera posições aleatórias para os tesouros
    srand(0);                                   //inicializa semente aleatória /OBS trocar para srand(time=NULL) depois de pronto
    unsigned int x, y;                          //posições dos tesouros

    for (int i = 0; i < 8; i++)
    {
        x = rand() % 8;
        y = rand() % 8;

        //Identifica qual tesouro (i) está na posição marcada
        tabuleiro->posicoes[x][y] = COM_TESOURO_NAO_VISITADA;
        tabuleiro->posicao_tesouro[i].x = x;
        tabuleiro->posicao_tesouro[i].y = y;
    }

    return tabuleiro;
}

void destroi_tabuleiro(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador)
{
    free (tabuleiro);
    free (jogador);
}

struct jogador_t* cria_jogador()
{
    struct jogador_t *jogador = malloc(sizeof(struct jogador_t));

    //Inicializações
    jogador->pos_x = 0;
    jogador->pos_y = 0;

    //Zera o bitmap dos tesouros encontrados (não tem nenhum ainda)
    for (int i = 0; i < 7; i++)
        jogador->tesouros[i] = 0;
    
    return jogador;
}

void imprime_tabuleiro(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador)
{
    for (int k = 7; k >= 0; k--)
    {
        //Topo dos quadradinhos
        for (int i = 0; i < 8; i++)
            printf("┌────┐ ");
        printf("\n");

        //Meio dos quadradinhos
        for (int i = 0; i < 8; i++)
        {
            if (tabuleiro->posicoes[k][i] == COM_TESOURO_VISITADA)
                printf("│ 🎖️  │ ");
            else if (tabuleiro->posicoes[k][i] == SEM_TESOURO_VISITADA)
                printf("│ ❌  │ ");   //mudar
            else if ((k == jogador->pos_x) && (i == jogador->pos_y))
                printf("│ 💃 │ ");
                else
                printf("│ ⚜️  │ ");
        }
        printf("\n");

        //Inferior dos quadradinhos
        for (int i = 0; i < 8; i++)
            printf("└────┘ ");
        printf("\n");
    }
}