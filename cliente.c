#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include "sock.h"
#include "pacote.h"

int main() {

    // criamos um socket com interface das maquinas virtuais
    int sock = cria_raw_socket("veth0");
  
    // cria pacote para enviar mensagens
    pacote_t *pack_send = cria_pacote();
    pacote_t *pack_recv = cria_pacote();

    // inicia conexao enviando um pacote para sincronizar com o servidor
    escreve_pacote(pack_send, SYN, 0, 0, NULL);
    envia_pacote(sock, pack_send);
    espera_ack(sock, pack_send, pack_recv);


    recebe_dados(sock, pack_send, pack_recv);


    destroi_pacote(pack_send);
    destroi_pacote(pack_recv);
    close(sock);

    return 0;
}