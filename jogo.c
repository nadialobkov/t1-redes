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

    // inicializa posicao jogador
    tabuleiro->posicao_jogador.x = 0;
    tabuleiro->posicao_jogador.y = 0;

    // tesouros encontrados
    tabuleiro->tesouros = 0;

    return tabuleiro;
}

void destroi_tabuleiro(struct tabuleiro_t *tabuleiro)
{
    free (tabuleiro);
}

struct jogador_t* cria_jogador()
{
    struct jogador_t *jogador = malloc(sizeof(struct jogador_t));

    //Inicializações
    jogador->pos_x = 0;
    jogador->pos_y = 0;

    // zera quantidade de tesouros encontrados
    jogador->tesouros = 0;

    jogador->casas_percorridas = 1;     //Não é zero por que ele já nasce em uma casa

    // inicializa mapa pessoal
    for (unsigned int i = 0; i < 8; i++)
        for (unsigned int j = 0; j < 8; j++)
            jogador->mapa[i][j] = NAO_VISITADA;
    
    return jogador;
}

void destroi_jogador(struct jogador_t *jogador) {
    free (jogador);
}

void imprime_mapa(struct jogador_t *jogador)
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
            else if (jogador->mapa[i][k] == COM_TESOURO_VISITADA)
                printf("│ 🎖️  │ ");
            else if (jogador->mapa[i][k] == SEM_TESOURO_VISITADA)
                printf("│ ❤️  │ ");   //mudar
            else
                printf("│ 🤍 │ ");
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

    // Lê o primeiro byte
    if (read(STDIN_FILENO, &direcao[0], 1) < 1) {
        restaura_canonico(&velha_configuracao);
        return 0;
    }

    // Se for ESC (27), tenta ler os dois próximos
    if (direcao[0] == '\x1b') {
        read(STDIN_FILENO, &direcao[1], 1);  // Espera '['
        read(STDIN_FILENO, &direcao[2], 1);  // Espera A/B/C/D
    }

    restaura_canonico(&velha_configuracao);

    // if (direcao[0] != '\x1b')
    // {
    //     printf("Tecla Inválida!\n");
    //     return 0;   //Movimento Inválido - Não pressionou uma seta
    // }        

    if (strcmp(direcao, "\x1b[A") == 0)
        return MOVE_CIMA;
    else if (strcmp(direcao, "\x1b[B") == 0)
        return MOVE_BAIXO;
    else if (strcmp(direcao, "\x1b[C") == 0)
        return MOVE_DIR;
    else if (strcmp(direcao, "\x1b[D") == 0)
        return MOVE_ESQ;
    else
    {
        printf("Tecla Inválida!\n");
        return 0;   //Movimento Inválido - Não pressionou uma seta
    }
}

void atualiza_jogador(struct jogador_t *jogador, unsigned int direcao, unsigned int estado)
{
    switch (direcao)
    {
        //Direita
        case MOVE_DIR:
            jogador->pos_x++;
            jogador->casas_percorridas++;
            break;

        //Cima
        case MOVE_CIMA:
            jogador->pos_y++;
            jogador->casas_percorridas++;
            break;

        //Baixo
        case MOVE_BAIXO:
            jogador->pos_y--;
            jogador->casas_percorridas++;
            break;

        //Esquerda
        case MOVE_ESQ:
            jogador->pos_x--;
            jogador->casas_percorridas++;
            break;
    }

    // atualiza mapa
    jogador->mapa[jogador->pos_x][jogador->pos_y] = estado;
    // se achou um tesouro
    if (estado == COM_TESOURO_VISITADA) {
        jogador->tesouros++;
    }

    return;
}

unsigned int movimenta_jogador(struct tabuleiro_t *tabuleiro, unsigned int direcao)
{
    switch (direcao)
    {
        //Direita
        case MOVE_DIR:
            if (tabuleiro->posicao_jogador.x == 7)
                return 0;                   //Não é possível movimentar
            
            (tabuleiro->posicao_jogador.x)++;
            break;

        //Cima
        case MOVE_CIMA:
            if (tabuleiro->posicao_jogador.y == 7)
                return 0;                   //Não é possível movimentar
            
            (tabuleiro->posicao_jogador.y)++;
            break;

        //Baixo
        case MOVE_BAIXO:
            if (tabuleiro->posicao_jogador.y == 0)
                return 0;                   //Não é possível movimentar
            
            (tabuleiro->posicao_jogador.y)--;
            break;

        //Esquerda
        case MOVE_ESQ:
            if (tabuleiro->posicao_jogador.x == 0)
                return 0;                   //Não é possível movimentar
            
            (tabuleiro->posicao_jogador.x--);
            break;
    }

    return 1;                               //Foi possível movimentar
}

int encontrou_tesouro(struct tabuleiro_t *tabuleiro)
{
    int i = 0;

    //Se houver tesouro não visitado
    if (tabuleiro->posicoes[tabuleiro->posicao_jogador.x][tabuleiro->posicao_jogador.y] == COM_TESOURO_NAO_VISITADA)
    {
        //Identifica qual foi o tesouro encontrado
        //Vai sair do laço quando encontrar o id do tesouro que temos as coordenadas
        while ((tabuleiro->posicao_tesouro[i].x != tabuleiro->posicao_jogador.x) || (tabuleiro->posicao_tesouro[i].y != tabuleiro->posicao_jogador.y))
            i++;

        //Atualiza o tabuleiro
        tabuleiro->posicoes[tabuleiro->posicao_jogador.x][tabuleiro->posicao_jogador.y] = COM_TESOURO_VISITADA;
        
        // conta tesouro encontrado
        tabuleiro->tesouros++;

        // retorna numero do tesouro encontrado
        return i;
    }

    if (tabuleiro->posicoes[tabuleiro->posicao_jogador.x][tabuleiro->posicao_jogador.y] == COM_TESOURO_VISITADA)
        return -1;       //Porque já tem o tesouro, então não encontrou nada novo

    tabuleiro->posicoes[tabuleiro->posicao_jogador.x][tabuleiro->posicao_jogador.y] = SEM_TESOURO_VISITADA;

    //Não tinha tesouro na posição
    return -1;
}

char *nome_tesouro(int num_tesouro) 
{
    switch (num_tesouro)
    {
        case 0:
            return TES_0;
            break;
        case 1:
            return TES_1;
            break;
        case 2:
            return TES_2;
            break;
        case 3:
            return TES_3;
            break;
        case 4:
            return TES_4;
            break;
        case 5:
            return TES_5;
            break;
        case 6:
            return TES_6;
            break;
        case 7:
            return TES_7;
            break;
        
        default:
            return NULL;
            break;
    }
}

unsigned int procurando_tesouros(struct jogador_t *jogador)
{
    if (jogador->tesouros >= 8)
        return 0;       //encontrou os 8 tesouros
    
    return 1;           //ainda procurando
}

unsigned int procurando_tesouros_tabuleiro(struct tabuleiro_t *tabuleiro)
{
    if (tabuleiro->tesouros >= 8)
        return 0;       //encontrou os 8 tesouros
    
    return 1;           //ainda procurando
}

void mensagem_vitoria(struct jogador_t *jogador)
{
    printf("            ⚜️ 🏆 𝕍𝕠𝕔ê 𝕍𝕖𝕟𝕔𝕖𝕦!!! 🏆⚜️\n");
    printf("      ┌──────────────────────────────────┐ \n");
    printf("      │ Número de Movimentos: %d         │ \n", jogador->casas_percorridas);
    printf("      └──────────────────────────────────┘ \n");
}

void limpa_terminal() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void infos_jogo(struct tabuleiro_t *tabuleiro)
{
    printf("\n Informações Do Jogo - Caça ao Tesouro\n");
    printf("┌─────────────────────────────────────────┐ \n");
    printf("│ Número de Tesouros encontrados: %d       │ \n", tabuleiro->tesouros);
    printf("│                                         │\n");
    printf("│ Posições dos Tesouros:                  │ \n");
    for (int i = 0; i < 8; i++)
        printf("│ ID: %d -> (%d, %d)                         │\n", i, tabuleiro->posicao_tesouro[i].x, tabuleiro->posicao_tesouro[i].y);
    
    printf("│                                         │\n");
    printf("│ Posicao jogador: (%d, %d)                 │\n", tabuleiro->posicao_jogador.x, tabuleiro->posicao_jogador.y);
    printf("└─────────────────────────────────────────┘ \n");
}