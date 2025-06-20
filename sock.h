#ifndef _SOCK_H__
#define _SOCK_H_


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


// recebe sockets, pacotes (previamente alocados) e nome do arquivo a ser enviado
// eh enviado o nome do arquivo, tamanho e em seguida os dados do arquivo
// o arquivo eh quebrado em partes (sequencializadas) para entrar no campo de dados
// ao final eh enviado uma mensagem de fim de arquivo
// todas as mensagens esperam um ACK de resposta
void envia_dados(int sock, pacote_t *pack_send, pacote_t *pack_recv, char *nome);






#endif