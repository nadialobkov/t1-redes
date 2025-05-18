#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include "sock.h"

int main() {

    // criamos um socket com interface das maquinas virtuais
    int sock = cria_raw_socket("veth0");

    struct pacote *pack = malloc(sizeof(struct pacote));

    // cliente vai enviar mensagens
    while (1) {
        // coloca marcador de inicio
        pack->marcador = 0x7e;

        printf("digite sua mensagem: ");
        fgets(pack->dados, 128, stdin);
        
        //Testando o calcula_checksum --------------------------------------------------
        //calcula o tamanho do que foi digitado
        int aux = strlen(pack->dados);
        pack->tam = aux;
        int checksum = calcula_checksum(pack);
        printf("o tamanho da mensagem é: %d\n", pack->tam);
        printf("checksum = %d\n", pack->checksum);
        //-------------------------------------------------------------------------------

        printf("esta eh a mensagem que voce enviou: %s\n", pack->dados);

        ssize_t envio = send(sock, pack, 132, 0);
        if (envio >= 0) {
            printf("%d bytes enviados\n", envio);
        }
        else {
            printf("erro ao enviar mensagem\n");
        }
    }

    close(sock);

    return 0;
}