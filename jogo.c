#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
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

void desativa_modo_canonico(struct termios *old)
{
    struct termios nova;

    //Lê as configurações atuais do terminal e salva
    tcgetattr(STDIN_FILENO, old);
    nova = *old;

    //Entra em modo não canônico e o que é digitado não é ecoado na tela
    //Junta os bits das flags em um or bit a bit
    //Inverte todos os bits do resultado com ~
    //Faz um and bit a bit e grava o resultado (zera os bits de ICANON e ECHO, mas preserva os outros)
    nova.c_lflag &= ~(ICANON | ECHO);

    //Altera o comportamento do terminal para a nova configuração
    tcsetattr(STDIN_FILENO, TCSANOW, &nova);
}

void restaura_canonico(struct termios *old)
{
    tcsetattr(STDIN_FILENO, TCSANOW, old);
}

unsigned int le_movimento()
{
    char direcao[4] = {0};               //3 bytes para a direção + \0
    struct termios velha_configuracao;

    desativa_modo_canonico(&velha_configuracao);

    //Lê 1 byte três vezes (as setas tem 3 bytes cada)
    fread(direcao, 1, 3, stdin);

    if (strcmp(direcao, "\x1b[A") == 0)
        return CIMA;
    else if (strcmp(direcao, "\x1b[B") == 0)
        return BAIXO;
    else if (strcmp(direcao, "\x1b[C") == 0)
        return DIREITA;
    else if (strcmp(direcao, "\x1b[D") == 0)
        return ESQUERDA;
    else
        return 0;   //Movimento Inválido - Não pressionou uma seta

    restaura_canonico(&velha_configuracao);
}

unsigned int movimenta_jogador(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador, unsigned int direcao)
{
    switch (direcao)
    {
        //Direita
        case DIREITA:
            if (jogador->pos_x == 7)
                return 0;                   //Não é possível movimentar
            
            jogador->pos_x++;
            break;

        //Cima
        case CIMA:
            if (jogador->pos_y == 7)
                return 0;                   //Não é possível movimentar
            
            jogador->pos_y++;
            break;

        //Baixo
        case BAIXO:
            if (jogador->pos_y == 0)
                return 0;                   //Não é possível movimentar
            
            jogador->pos_y--;
            break;

        //Esquerda
        case ESQUERDA:
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

    if (tabuleiro->posicoes[jogador->pos_x][jogador->pos_y] == COM_TESOURO_VISITADA)
        return 0;       //Porque já tem o tesouro, então não encontrou nada novo

    tabuleiro->posicoes[jogador->pos_x][jogador->pos_y] = SEM_TESOURO_VISITADA;

    //Não tinha tesouro na posição
    return 0;
}