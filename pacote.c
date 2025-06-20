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
    
    printf("Checksum OK!\n");

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

    return;
}

// espera pacotes e verifica marcador de inicio
// escreve em 'pack' o pacote recebido
uint8_t recebe_pacote(int sock, pacote_t *pack) {

    int bytes_lidos = 0;
    uint8_t marc = 0;

    // enquanto nao for o marcador de inicio
    while (marc != MARC) {
        // vai ficar esperando mensagens ate receber algo
        do {
            bytes_lidos = recv(sock, pack, TAM_PAC, 0);
        } while (bytes_lidos <= 0);

        marc = pack->marcador;
    }

    return (pack->tam);
}

// espera ate receber um pacote do tipo ACK
// se recebeu outro tipo, reenvia a mesma mensagem
// pack_send => pacote que contem a mensagem a ser enviada
// pack_recv => pacote por onde vai recever a mensagem
void espera_ack(int sock, pacote_t *pack_send, pacote_t *pack_recv) {

    while (recebe_pacote(sock, pack_recv) != ACK) {
        envia_pacote(sock, pack_send); // reenvia
    }
    return;
}
