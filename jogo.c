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

    //Gera posições aleatórias para os tesouros
    srand(0);                                   //inicializa semente aleatória /OBS trocar para srand(time=NULL) depois de pronto
    unsigned int x, y;                          //posições dos tesouros

    for (int i = 0; i < 8; i++)
    {
        x = rand() % 8;
        y = rand() % 8;

        //TOMAR CUIDADO COM ESSE WHILE
        while (tabuleiro->posicoes[x][y] == COM_TESOURO_NAO_VISITADA)
        {
            //Já tem tesouro nessa posição - precisa sortear novamente
            x = rand() % 8;
            y = rand() % 8;
        }

        //Identifica qual tesouro (i) está na posição marcada
        tabuleiro->posicoes[x][y] = COM_TESOURO_NAO_VISITADA;
        tabuleiro->posicao_tesouro[i].x = x;
        tabuleiro->posicao_tesouro[i].y = y;

        //Isso ajuda a debugar
        //printf("tesouro[%d].x = %d\ntesouro[%d].y = %d\n", i, x, i, y);
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
    printf("                 ⚔️ C𝕒ç𝕒 𝕒𝕠 𝕋𝕖𝕤𝕠𝕦𝕣𝕠⚔️\n");
    printf("⚔️ 💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠⚔️\n");
    for (int k = 7; k >= 0; k--)
    {
        //Topo dos quadradinhos
        for (int i = 0; i < 8; i++)
            printf("┌────┐ ");
        printf("\n");

        //Meio dos quadradinhos
        for (int i = 0; i < 8; i++)
        {
            if ((k == jogador->pos_y) && (i == jogador->pos_x))
                printf("│ 💃 │ ");
            else if (tabuleiro->posicoes[i][k] == COM_TESOURO_VISITADA)
                printf("│ 🎖️  │ ");
            else if (tabuleiro->posicoes[i][k] == SEM_TESOURO_VISITADA)
                printf("│ ❌ │ ");   //mudar
            else
                printf("│ ⚜️  │ ");
        }
        printf("\n");

        //Inferior dos quadradinhos
        for (int i = 0; i < 8; i++)
            printf("└────┘ ");
        printf("\n");
    }
    printf("⚔️ 💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠💠⚔️\n");
}

unsigned int movimenta_jogador(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador, unsigned int direcao)
{
    switch (direcao)
    {
        //Direita
        case 10:
            if (jogador->pos_x == 7)
                return 0;                   //Não é possível movimentar
            
            jogador->pos_x++;
            break;

        //Cima
        case 11:
            if (jogador->pos_y == 7)
                return 0;                   //Não é possível movimentar
            
            jogador->pos_y++;
            break;

        //Baixo
        case 12:
            if (jogador->pos_y == 0)
                return 0;                   //Não é possível movimentar
            
            jogador->pos_y--;
            break;

        //Esquerda
        case 13:
            if (jogador->pos_x == 0)
                return 0;                   //Não é possível movimentar
            
            jogador->pos_x--;
            break;
    }

    return 1;                               //Foi possível movimentar
}

unsigned int encontrou_tesouro(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador)
{
    unsigned int i = 0;

    //Se houver tesouro não visitado
    if (tabuleiro->posicoes[jogador->pos_x][jogador->pos_y] == COM_TESOURO_NAO_VISITADA)
    {
        //Identifica qual foi o tesouro encontrado
        //Vai sair do laço quando encontrar o id do tesouro que temos as coordenadas
        while ((tabuleiro->posicao_tesouro[i].x != jogador->pos_x) || (tabuleiro->posicao_tesouro[i].y != jogador->pos_y))
            i++;
        
        //Atualiza o bitmap de tesouros do jogador
        jogador->tesouros[i] = 1;

        //Atualiza o tabuleiro
        printf("to aqui\n");
        tabuleiro->posicoes[jogador->pos_x][jogador->pos_y] = COM_TESOURO_VISITADA;
        
        return 1;
    }

    tabuleiro->posicoes[jogador->pos_x][jogador->pos_y] = SEM_TESOURO_VISITADA;

    //Não tinha tesouro na posição
    return 0;
}