#ifndef _JOGO_H_
#define _JOGO_H_

//Estados das posições
#define NAO_VISITADA 0
#define SEM_TESOURO_VISITADA 1
#define COM_TESOURO_NAO_VISITADA 2
#define COM_TESOURO_VISITADA 3

//Direções
#define DIREITA 10
#define CIMA 11
#define BAIXO 12
#define ESQUERDA 13

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
    unsigned int casas_percorridas;     //quantidade de casas que o jogador já passou
};

typedef struct termios termios;

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

//Desativa o modo canonico para não precisar digitar o enter depois de pressionar uma tecla
void desativa_modo_canonico(struct termios *old);

//Reativa o modo canonico
void restaura_canonico(struct termios *old);

//Captura a seta pressionada pelo usuário
//Retorno: um inteiro que representa a posição recebida
unsigned int le_movimento();

//Verifica a possibilidade de mudar de posição
//Caso seja possível, mudar de posição
//Caso não seja, ficar no lugar e devolver 0
//(A ideia é colocar o campo dos dados na direção)
unsigned int movimenta_jogador(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador, unsigned int direcao);

//Trata o caso do jogador encontrar um tesouro
//Retorno: 1 caso encontrou o tesouro e 0 caso contrário
unsigned int encontrou_tesouro(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador);

//Pode ser usado para o controle do laço das movimentações
//Retorna 1 enquanto não tiver encontrado todos os tesouros e 0 quando tiver encontrado todos
unsigned int procurando_tesouros(struct jogador_t *jogador);

//Para encerrar o jogo
//Mostrar na tela do cliente uma mensagem de vitória
void mensagem_vitoria(struct jogador_t *jogador);

//Limpa a tela (usar se precisar)
void limpa_terminal();

//Imprime as informações gerenciais do jogo
//É chamada no servidor
void infos_jogo(struct tabuleiro_t *tabuleiro, struct jogador_t *jogador);


#endif