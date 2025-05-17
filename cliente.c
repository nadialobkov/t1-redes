#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>

int main() {

    // criamos um socket com interface loopback
    int sock = cria_raw_socket("lo");


    // cliente vai enviar mensagens
    while (1) {
        // codigo para enviar mensagens
    }

    close(sock);

    return 0;
}