#ifndef _SOCK_H__
#define _SOCK_H_

#include "pacote.h"

// BIBLIOTECA PARA TRATAR COM RAW SOCKETS

// ==========================================================================================================

// FUNCOES

// funcao de criacao de raw socket (fornecida no enunciado)
// retorna file descriptor do socket
int cria_raw_socket(char* nome_interface_rede);


// detectna o tipo dos dados (extensão)
// retorna tipo (IMG, VIDEO, TXT) ou ERRO em caso de erro
uint8_t devolve_extensao(char *caminho_arquivo);

//Mostra o arquivo na tela
void exibe_arquivo(const char *caminho_arquivo);


// funcao recebe sockets, pacotes (previamente alocados) e nome do arquivo a ser enviado
// eh enviado o nome do arquivo, tamanho e em seguida os dados do arquivo
// o arquivo eh quebrado em partes (sequencializadas) para entrar no campo de dados
// ao final eh enviado uma mensagem de fim de arquivo
// todas as mensagens esperam um ACK de resposta
void envia_dados(int sock, pacote_t *pack_send, pacote_t *pack_recv, char *nome);

// funcao recebe socket e pacotes (previamente alocados)
// espera receber pacotes contendo o nome, tamanho e dados do arquivo (com finalizador)
// cria esse arquivo recebido em partes na memoria e exibe ele
// envia ACKS e NACKS, seguindo o protocolo
void recebe_dados(int sock, pacote_t *pack_send, pacote_t *pack_recv);



#endif