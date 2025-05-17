#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include "sock.h"

int main() {

    // criamos um socket com interface loopback
    int sock = cria_raw_socket("lo");

    // vamos fazer um loop para ficar esperando mensagens
    while (1) {
        // codigo aqui de recebimento e tratamento de mensagens
    }

    close(socket);
    return 0;
}