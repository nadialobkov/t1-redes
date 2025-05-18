#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>

#include "sock.h"
 

// funcao de criacao de raw socket passada no enunciado do trabalho

int cria_raw_socket(char* nome_interface_rede) {
    // Cria arquivo para o socket sem qualquer protocolo
    int soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (soquete == -1) {
        fprintf(stderr, "Erro ao criar socket: Verifique se você é root!\n");
        exit(-1);
    }
 
    int ifindex = if_nametoindex(nome_interface_rede);
 
    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(ETH_P_ALL);
    endereco.sll_ifindex = ifindex;
    // Inicializa socket
    if (bind(soquete, (struct sockaddr*) &endereco, sizeof(endereco)) == -1) {
        fprintf(stderr, "Erro ao fazer bind no socket\n");
        exit(-1);
    }
 
    struct packet_mreq mr = {0};
    mr.mr_ifindex = ifindex;
    mr.mr_type = PACKET_MR_PROMISC;
    // Não joga fora o que identifica como lixo: Modo promíscuo
    if (setsockopt(soquete, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
        fprintf(stderr, "Erro ao fazer setsockopt: "
            "Verifique se a interface de rede foi especificada corretamente.\n");
        exit(-1);
    }
 
    return soquete;
}

// ------------------------------------------------------------------------------------------------

//Calcula checksum --------------------------------------------------------------------------------
//Campos: tamanho + sequência + tipo + dados
//OBS: Soma o campo dos dados somente até a quantidade indicada pelo tamanho
unsigned int calcula_checksum(struct pacote *pack)
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

    return checksum;
}
//-------------------------------------------------------------------------------------------------

//Verifica o campo do checksum da mensagem recebida -----------------------------------------------
unsigned int verifica_checksum(struct pacote *pack)
{

}
//-------------------------------------------------------------------------------------------------