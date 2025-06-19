#ifndef _JOGO_H_
#define _JOGO_H_

//Estrutura Tabuleiro - Estático, porque as posições não irão passar de 64 e são 8 tesouros
struct tabuleiro_t {
    unsigned int posicoes[64][64];      //matriz de posições (são 64 posições disponíveis)
    unsigned int pos_x_tesouro[8];      //coordenadas x das posições com tesouro
    unsigned int pos_y_tesouro[8];      //coordenadas y das posições com tesouro
    unsigned int pos_x_vista[64];       //coordenadas x das posições já vistas
    unsigned int pos_y_vista[64];       //coordenadas y das posições já vistas
    char *tesouros[8];                  //todos os tesouros do tabuleiro
};

//Estrutura Jogador
struct jogador_t{
    unsigned int pos_x;                 //coordenada x da posição atual
    unsigned int pos_y;                 //coordenada y da posição atual
    char *tesouros[8];                  //tesouros que o jogador encontrou
};

//Funções

//Cria e inicializa o tabuleiro 8x8
//Retorno: um ponteiro para o tabuleiro
struct tabuleiro_t* cria_tabuleiro();

//Cria um jogador no ponto 0x0 e o inicializa
//Retorno: um ponteiro para o jogador
struct jogador_t* cria_jogador();

//Imprime na tela o grid do tabuleiro e o jogador na posição 0x0
//(canto inferior esquerdo)
void imprime_tabuleiro(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador);


#endif