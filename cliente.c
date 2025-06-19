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

    struct pacote *pack = malloc(sizeof(struct pacote));

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
        ssize_t envio = send(sock, pack, 132, 0);
        if (envio >= 0) {
            printf("%d bytes enviados\n", envio);

                // aloca um espaco para um um vetor de pacotes que vai ter arquivo de no max 128 * 10 000 bytes
                struct pacote **packets = malloc(10000 * sizeof(struct pacote *));

                uint8_t i = 0;
                packets[i] = malloc(sizeof(struct pacote));

                while (1) {
                    // recebe pacotes
                    recv(sock, packets[i], 131, 0);
                    if (packets[i]->marcador == MARC){ // verifica marcador
                        if (packets[i]->tipo == DADOS) {
                            i++;
                            packets[i] = malloc(sizeof(struct pacote));
                        }
                        if (packets[i]->tipo == FIM) {
                            interpreta_pacotes_dados(packets, i, "arquivo.txt");
                            printf("recebeuu!\n");
                            break;
                        }
                    }
                }

        }
        else {
            printf("erro ao enviar mensagem\n");
        }
        struct pacote *ack = malloc(sizeof(struct pacote));
        ssize_t ack_recebido = recv(sock, ack, 132, 0);
        //colocar em uma função
        if (ack_recebido > 0)
        {
            if (ack->tipo == NACK)
            {
                //tratar (enviar novamente)
                envio = send(sock, pack, 132, 0);
                if (envio <= 0)
                    printf("Erro ao enviar mensagem.\n");
            }
            else if (ack->tipo == ERRO)
            {
                //tratar
            }
            else if (ack->tipo == ACK)
            {
                //espera o ack + ok
            }
            else
            {
                //mensagem recebida e processada, então podemos enviar as próximas
                continue;
            }
        }
        else
        {
            //devemos enviar novamente (a mensaagem de resposta foi perdida)
            //
        }
    }

    close(sock);

    return 0;
}