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
    struct pacote *resposta_servidor = malloc(sizeof(struct pacote));

    // cliente vai enviar mensagens
    while (1) {
        // coloca marcador de inicio
        pack->marcador = 0x7e;

        printf("digite caminho do arquivo que deseja receber: ");
        fgets(pack->dados, 128, stdin);

        //unsigned int tamanho_string = strlen(pack->dados);
        //pack->dados[tamanho_string - 1] = '\0';
        
        //Testando o calcula_checksum --------------------------------------------------
        //calcula o tamanho do que foi digitado
        int aux = strlen(pack->dados);
        pack->tam = aux;
        calcula_checksum(pack);
        printf("o tamanho da mensagem é: %d\n", pack->tam);
        printf("checksum = %d\n", pack->checksum);
        //-------------------------------------------------------------------------------

        printf("esta eh a mensagem que voce enviou: %s\n", pack->dados);

        //Ainda precisa tratar se recebeu ack, nack ou erro 
        ssize_t envio = send(sock, pack, 131, 0);
        if (envio >= 0) {
            // printf("%d bytes enviados\n", envio);

            //     // aloca um espaco para um um vetor de pacotes que vai ter arquivo de no max 128 * 10 000 bytes
            //     struct pacote **packets = malloc(10000 * sizeof(struct pacote *));

            //     uint8_t i = 0;
            //     packets[i] = malloc(sizeof(struct pacote));

            //     while (1) {
            //         // recebe pacotes
            //         printf("recebendo pacotes\n");
            //         recv(sock, packets[i], 131, 0);
            //         printf("indo testar marcador\n");
            //         printf("marcador[%d] = %d\n", i, packets[i]->marcador);
            //         if (packets[i]->marcador == MARC){ // verifica marcador
            //             printf("testando marcador\n");
            //             if (packets[i]->tipo == DADOS) {
            //                 //printf("pacote dados\n");
            //                 i++;
            //                 packets[i] = malloc(sizeof(struct pacote));
            //             }
            //             if (packets[i]->tipo == FIM) {
            //                 //printf("último pacote\n");
            //                 interpreta_pacotes_dados(packets, i, "arquivo.txt");
            //                 printf("recebeuu!\n");
            //                 break;
            //             }
            //         }
            //     }

        }
        else {
            printf("erro ao enviar mensagem\n");
        }
        
        printf("vou entrar no if dos tipos de mensagem\n");
        // trata_ack_nack_erro(resposta_servidor, pack, sock, envio);
    }

    close(sock);

    return 0;
}