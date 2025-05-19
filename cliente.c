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

        printf("digite caminho do arquivo que deseja receber: ");
        fgets(pack->dados, 128, stdin);
        printf("esta eh a mensagem que voce enviou: %s\n", pack->dados);

        ssize_t envio = send(sock, pack, 132, 0);
        if (envio >= 0) {
            printf("%d bytes enviados\n", envio);

            
                struct pacote **packets = malloc(1000000); // max 1 MB 

                uint8_t i = 0;
                while (1) {
                    packets[i] = malloc(sizeof(struct pacote));
                    recv(sock, packets[i], 131, 0);
                    if (packets[i] == MARC){
                        i++;
                        if (packets[i]->tipo == FIM) {
                            interpreta_pacotes_dados(packets, i, "arquivo.txt");
                            break;
                        }
                    }

                }
        }
        else {
            printf("erro ao enviar mensagem\n");
        }
    }

    close(sock);

    return 0;
}