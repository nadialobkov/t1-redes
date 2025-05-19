#ifndef _SOCK_H__
#define _SOCK_H_


// BIBLIOTECA PARA TRATAR COM RAW SOCKETS

// ------------------------------------------------------------------------------------------------
// MACROS

// tipos de mensagens
#define ACK     0
#define NACK    1
#define OK      2   // ok + ack
// #define LIVRE 3
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


// ------------------------------------------------------------------------------------------------
// ESTRUTURA

#pragma pack(push, 1) // evita otimizacao de alinhamento de bytes (garante pacote de 131 bytes)

struct pacote {
    // cabecalho do pacote
    uint8_t marcador;   // 8 bits para marcador de inicio
    uint8_t tam : 7;    // 7 bits para tamanho dos dados
    uint8_t seq : 5;    // 5 bits para campo de sequencia
    uint8_t tipo : 4;   // 4 bits para tipo da mensagem   
    uint8_t checksum;   // 8 bits para checksum

    //dados
    uint8_t dados[127]; // vetor de bytes de dados
};

#pragma pack(pop); // restaura alinhamento padrao

// ------------------------------------------------------------------------------------------------
// FUNCOES

// funcao de criacao de raw socket (fornecida no enunciado)
// retorna file descriptor do socket
int cria_raw_socket(char* nome_interface_rede);

//Calcula a soma dos bits dos campos tamanho, sequência, tipo e dados
//Guarda o valor no pacote
void calcula_checksum(struct pacote *pack);

//Verifica se o checksum está correto (momento que recebe o pacote)
//Retorna 1 se estiver tudo certo e 0 se houver erro
unsigned int verifica_checksum(struct pacote *pack);

//Detecta o tipo dos dados
//Retorno: void (altera somente o campo pack->tipo da struct)
void detecta_tipo(struct pacote *pack, char *caminho_arquivo);



#endif