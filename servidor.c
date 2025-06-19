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
    int sock = cria_raw_socket("veth1");

    pacote_t *pack_recv = cria_pacote();

    recebe_pacote(sock, pack_recv);
    
    // adiciona \0 para poder imprimir
    pack_recv->dados[pack_recv->tam] = '\0';
    printf("mensagem recebida: %s", pack_recv->dados);

    destroi_pacote(pack_recv);
    close(sock);

    return 0;
}