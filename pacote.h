#ifndef _PACOTE_H_
#define _PACOTE_H_


// ==========================================================================================================

// MACROS

// tipos de mensagens
#define ACK     0
#define NACK    1
#define OK      2   // ok + ack
#define SYN     3   // conexao inicial
#define TAM     4
#define DADOS   5
#define TEXT    6   // texto + ack + nome
#define VIDEO   7   // video + ack + nome
#define IMG     8   // imagem +ack + nome
#define FIM     9   
#define DIR     10  // desloca para direita
#define CIMA    11  // desloca para cima     
#define BAIXO   12  // desloca para baixo
#define ESQ     13  // desloca para esquerda
// #define LIVRE 14 
#define ERRO    15


// codigos de erros
#define SEM_PERM    0   // sem permissao de acesso
#define SEM_ESP     1   // espaco insuficiente
#define MARC_ERRO   2   // marcador de início errado

// tamanhos
#define TAM_MAX 127     // tamanho do campo de dados
#define TAM_PAC 131     // tamanho do pacote

// valor marcador de inicio
#define MARC 0x7e

// ==========================================================================================================

// ESTRUTURA

#pragma pack(push, 1) // evita otimizacao de alinhamento de bytes (garante pacote de 131 bytes)

typedef struct pacote_t {
    // cabecalho do pacote
    uint8_t marcador;   // 8 bits para marcador de inicio
    uint8_t tam : 7;    // 7 bits para tamanho dos dados
    uint8_t seq : 5;    // 5 bits para campo de sequencia
    uint8_t tipo : 4;   // 4 bits para tipo da mensagem   
    uint8_t checksum;   // 8 bits para checksum

    //dados
    uint8_t dados[TAM_MAX]; // vetor de bytes de dados
} pacote_t;

#pragma pack(pop); // restaura alinhamento padrao


// ==========================================================================================================

// FUNCOES

// retorna um pacote alocado dinamicamente com todos os campos vazios (zerados)
pacote_t *cria_pacote();

// libera espaco alocado pelo pacote
void destroi_pacote(pacote_t *pack);


// calcula a soma dos bits dos campos tamanho, sequência, tipo e dados
//Guarda o valor no pacote
void calcula_checksum(pacote_t *pack);

// verifica se o checksum está correto (momento que recebe o pacote)
// retorna 1 se estiver tudo certo e 0 se houver erro
unsigned int verifica_checksum(pacote_t *pack);


// escreve as informacoes passadas nos campos do pacote
// a estrutura deve estar previamente alocada (nao aloca)
// adiciona marcador de inicio e realiza checksum
void escreve_pacote(pacote_t *pack, uint8_t tipo, uint8_t tam, uint8_t seq, uint8_t *dados); 

// envia a mensagem contida no pacote 'pack' atraves do socket
// exibe possiveis erros
void envia_pacote(int sock, pacote_t *pack);

// recebe um pacote atraves do socket e escreve ele em 'pack'
// esta funcao espera ate receber uma mensagem valida (verificando o marcador de inicio)
void recebe_pacote(int sock, pacote_t *pack);


#endif