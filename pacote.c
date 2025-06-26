#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <unistd.h>         //Para reproduzir imagens e vídeos
#include <sys/wait.h>       //Para reproduzir imagens e vídeos
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "sock.h"
#include "pacote.h"
#include "timer.h"


// -----------------------------------------------------------------------------------------------

// retorna um pacote alocado dinamicamente com todos os campos vazios (zerados)
pacote_t *cria_pacote() {

    pacote_t *pack = malloc(sizeof(pacote_t));
    if (!pack) {
        perror("Erro ao alocar pacote");
        return NULL;
    }

    // zera toda estrutura
    memset(pack, 0, sizeof(pacote_t));

    return (pack);
}


// libera espaco alocado pelo pacote
void destroi_pacote(pacote_t *pack) {

    if (!pack) {
        return;
    }

    free (pack);
    return;
}

// imprime informacoes do pacote (tipo, tamanho, dados)
void imprime_pacote(pacote_t *pack) {

    printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    if (!pack) {
        printf("Pacote inválido\n");
        return;
    }

    printf("Pacote: ");
    switch (pack->tipo) {
        case ACK: printf("ACK\n"); break;
        case NACK: printf("NACK\n"); break;
        case OK: printf("OK\n"); break;
        case SYN: printf("SYN\n"); break;
        case TAM: printf("TAM\n"); break;
        case DADOS: printf("DADOS\n"); break;
        case TEXT: printf("TEXT\n"); break;
        case VIDEO: printf("VIDEO\n"); break;
        case IMG: printf("IMG\n"); break;
        case FIM: printf("FIM\n"); break;
        case DIR: printf("DIR\n"); break;
        case CIMA: printf("CIMA\n"); break;
        case BAIXO: printf("BAIXO\n"); break;
        case ESQ: printf("ESQ\n"); break;
        case ERRO: printf("ERRO\n"); break;
        default: break;
    }

    printf("\tNúmero sequencia: %d\n", pack->seq);
    printf("\tTamanho: %d bytes\n", pack->tam);
    // copia dados para buffer para imprimir
    uint8_t buffer[pack->tam];
    strncpy(buffer, pack->dados, pack->tam);
    printf("\tDados:\n");
    // formatacao dados em hexa
    for (int i = 0; i < pack->tam; i ++) {
        if ((i % 16) == 0) {
            printf("\t\t");
        }
        printf("%2x ", buffer[i]);
        if ((i % 16) == 15) {
            printf("\n");
        }
    }
    printf("\n");

    return; 
}

// retorna o primeiro byte de dados
uint8_t dado_pacote(pacote_t *pack) {
    if (pack->tam == 1) {
        return (pack->dados[0]);
    }
    else {
        perror("Pacote não tem um único byte");
        return 0;
    }
}

// Calcula checksum --------------------------------------------------------------------------------
// Campos: tamanho + sequência + tipo + dados
// OBS: Soma o campo dos dados somente até a quantidade indicada pelo tamanho
void calcula_checksum(pacote_t *pack)
{
    unsigned int checksum;

    checksum = pack->tam + pack->seq + pack->tipo;

    //Soma os dados escritos
    for (int i = 0; i < pack->tam; i++)
        checksum += pack->dados[i];

    //Garante que o valor vai ter 8 bits 
    //Pega os 8 bits menos significativos
    //checksum AND 11111111 (por isso mantém somente os 8 bits menos significativos ligados)
    checksum = checksum & 0xFF;

    //Escreve o valor no campo especificado
    pack->checksum = checksum;

}

// Verifica o campo do checksum da mensagem recebida -----------------------------------------------
// *pack é a mensagem recebida
// retorna 1 em caso de sucesso e 0 caso houver algum erro no checksum
unsigned int verifica_checksum(pacote_t *pack)
{
    unsigned int checksum_original;

    //Armazena o valor inicial do checksum
    checksum_original = pack->checksum;

    //Zera temporariamente (para não interferir no cálculo)
    pack->checksum = 0;

    //Calcula o checksum da mensagem recebida
    calcula_checksum(pack);

    //Se não forem iguais, houve erro
    if (checksum_original != pack->checksum)
        return 0;
    

    return 1;
}



// escreve as informacoes passadas no pacote 'pack'
// prepara a mensagem adicionando marcador de inicio e checksum
void escreve_pacote(pacote_t *pack, uint8_t tipo, uint8_t tam, uint8_t seq, uint8_t *dados) {

    if (!pack) {
        perror("Não foi possível escrever no pacote");
        return;
    }

    // limpa pacote
    memset(pack, 0, sizeof(pacote_t));

    // adiciona marcador de inicio
    pack->marcador = MARC;

    pack->tipo = tipo;
    pack->tam = tam;
    pack->seq = seq;

    // copia 'tam' bytes para dados
    if (tam > 0) {
        memcpy(pack->dados, dados, tam);
    }

    calcula_checksum(pack);

    return;
}

// envia a mensagem escrita no pacote atraves do socket informado
void envia_pacote(int sock, pacote_t *pack) {

    if (!pack) {
        perror("Não foi possível enviar esse pacote");
        return;
    }

    // envia mensagem
    int ret = send(sock, pack, TAM_PAC, 0);

    // verifica retorno do send
    if (ret < 0) {
        perror("Erro ao enviar mensagem");
    }

    #ifdef DEBUG
    printf("Enviado: ");
    imprime_pacote(pack);
    #endif

    return;
}

// espera pacotes e verifica marcador de inicio
// escreve em 'pack' o pacote recebido
uint8_t recebe_pacote(int sock, pacote_t *pack) {

    int bytes_lidos = 0;
    uint8_t marc = 0;

    // enquanto nao for o marcador de inicio
    while (marc != MARC) {
        // vai ficar esperando mensagens ate receber algo ou dar timeout
        do {
            bytes_lidos = recv(sock, pack, TAM_PAC, 0);
            // se deu timeout, sai do laco retornando aviso de timeout
            if (deu_timeout()) {
                return TIMEOUT;
            }
        } while ((bytes_lidos <= 0));


        marc = pack->marcador;
    }

    #ifdef DEBUG
    printf("Recebido: ");
    imprime_pacote(pack);
    #endif

    return (pack->tipo);
}

// espera ate receber um pacote do tipo ACK
// se recebeu outro tipo, reenvia a mesma mensagem ou deu timeout 
// pack_send => pacote que contem a mensagem a ser enviada
// pack_recv => pacote por onde vai recever a mensagem
void espera_ack(int sock, pacote_t *pack_send, pacote_t *pack_recv) {

    while ((recebe_pacote(sock, pack_recv) != ACK)) {
        envia_pacote(sock, pack_send); // reenvia
    }
    return;
}

// faz a verificacao do checksum do pacote de recebimento (pack_recv)
// em caso de sucesso, envia um ACK e retorna 1
// caso contrario, envia NACK e retorna 0
uint8_t verifica_pacote(int sock, pacote_t *pack_send, pacote_t *pack_recv) {
    // faz a verificacao dos dados
    if (!verifica_checksum(pack_recv)) {
        // se houve erro no checksum, envia um nack
        escreve_pacote(pack_send, NACK, 0, 0, NULL);
        envia_pacote(sock, pack_send);
        return 0;
    }
    // se checksum esta correto, enviamos um ack
    else {
        escreve_pacote(pack_send, ACK, 0, 0, NULL);
        envia_pacote(sock, pack_send);
        return 1;
    }
}


// espera o recebimento do pacote valido
// recebe pacotes e faz sua verificacao com 'verifica_pacote' (logo, realiza o envio de ACKS e NACKS)
// retorna o tipo do pacote recebido quando for verificado com sucesso
uint8_t espera_pacote(int sock, pacote_t *pack_send, pacote_t *pack_recv) {

    uint8_t tipo = 0;
    do {
        do {
            tipo = recebe_pacote(sock, pack_recv);
        } while (tipo == TIMEOUT);
    } while (!verifica_pacote(sock, pack_send, pack_recv));
    
    return (pack_recv->tipo);
}