#ifndef _SOCK_H__
#define _SOCK_H_


// BIBLIOTECA PARA TRATAR COM RAW SOCKETS

// ==========================================================================================================

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

//Trata os casos de ACK, NACK e ERRO
//Retorno: os bytes enviados ou 0 em caso de ACK




#endif