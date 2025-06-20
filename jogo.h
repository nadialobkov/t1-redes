#ifndef _JOGO_H_
#define _JOGO_H_

//Estados das posições
#define NAO_VISITADA 0
#define SEM_TESOURO_VISITADA 1
#define COM_TESOURO_NAO_VISITADA 2
#define COM_TESOURO_VISITADA 3

//Estrutura Coordenadas
struct coordenadas_t {
    unsigned int x;
    unsigned int y;
};

//Estrutura Tabuleiro - Estático, porque as posições não irão passar de 64 e são 8 tesouros
struct tabuleiro_t {
    unsigned int posicoes[8][8];               //matriz de posições (são 64 posições disponíveis)
    struct coordenadas_t posicao_tesouro[8];   //coordenadas x e y dos tesouros
};

//Estrutura Jogador
struct jogador_t{
    unsigned int pos_x;                 //coordenada x da posição atual
    unsigned int pos_y;                 //coordenada y da posição atual
    unsigned int tesouros[8];           //bitmap dos tesouros que o jogador encontrou
};

//Funções

//Cria e inicializa o tabuleiro 8x8
//Retorno: um ponteiro para o tabuleiro
struct tabuleiro_t* cria_tabuleiro();

//Desaloca a memória usada no tabuleiro e no jogador
void destroi_tabuleiro(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador);

//Cria um jogador no ponto 0x0 e o inicializa
//Retorno: um ponteiro para o jogador
struct jogador_t* cria_jogador();

//Imprime na tela o grid do tabuleiro e o jogador na posição 0x0
//(canto inferior esquerdo)
void imprime_tabuleiro(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador);

//Verifica a possibilidade de mudar de posição
//Caso seja possível, mudar de posição
//Caso não seja, ficar no lugar e devolver 0
//(A ideia é colocar o campo dos dados na direção)
unsigned int movimenta_jogador(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador, unsigned int direcao);

//Trata o caso do jogador encontrar um tesouro
//Retorno: 1 caso encontrou o tesouro e 0 caso contrário
unsigned int encontrou_tesouro(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador);


#endif