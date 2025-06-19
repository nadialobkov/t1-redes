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

    // cria uma mensagem de teste
    char string[] = "oiii";
    escreve_pacote(pack_send, DADOS, 4, 0, string);

    envia_pacote(sock, pack_send);


    destroi_pacote(pack_send);
    close(sock);

    return 0;
}