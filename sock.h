#ifndef _SOCK_H__
#define _SOCK_H_


// BIBLIOTECA PARA TRATAR COM RAW SOCKETS

// ==========================================================================================================

// FUNCOES

// funcao de criacao de raw socket (fornecida no enunciado)
// retorna file descriptor do socket
int cria_raw_socket(char* nome_interface_rede);

//Detecta o tipo dos dados (extensão)
//Recebe um arquivo e retorna sua extensão
char* devolve_extensao(char *caminho_arquivo);

//Mostra o arquivo na tela
void exibe_arquivo(const char *caminho_arquivo);

// Prepara um vetor de pacotes de dados que formam um arquivo para serem enviados sequencialmente
// Retorno: vetor de pacotes
struct pacote **prepara_pacotes_dados(const char *caminho);

// Cria um arquivo no caminho indicado com os dados presentes no vetor de pacotes
// Retorno: 0 em caso de sucesso e valores negativos em caso de erro
uint8_t interpreta_pacotes_dados(struct pacote **packets, uint8_t tam, const char *caminho);

//Atribui o ack + formato do arquivo e coloca o nome dele no campo de dados
//Retorno: um pacote que é a mensagem de ACK
struct pacote* ack_format_arq(struct pacote *pack);

//Calcula o checksum
//Devolve erro em caso de falha no checksum
//Devolve erro caso não encontre o marcador de início
//Altera o campo interno do pacote
//Retorno: void
void verifica_pacote(struct pacote *pack, struct pacote *resposta_servidor);

//Trata os casos de ACK, NACK e ERRO
//Retorno: void (altera o campo interno do pacote)
void trata_ack_nack_erro(struct pacote *resposta_servidor, struct pacote *pack, int sock, ssize_t envio);



#endif