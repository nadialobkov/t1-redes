#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include "sock.h"

int main() {

    // criamos um socket com interface das maquinas virtuais 
    int sock = cria_raw_socket("veth1");

    struct pacote *pack = malloc(sizeof(struct pacote));
    printf("%ld\n", sizeof(struct pacote));
    struct pacote *nack = malloc(sizeof(struct pacote));

    // vamos fazer um loop para ficar esperando mensagens
    while (1) {
        // recebe dados
        ssize_t tam = recv(sock, pack, 132, 0);

        if (tam > 0) {
            // verifica marcador de inicio
            if (pack->marcador == 0x7e) {
                //alterei um bit para testar se a função encontra o erro
                //pack->dados[0] = pack->dados[0] ^ 0xFF;
                /*
                int verificaChecksum = verifica_checksum(pack);
                if (!verificaChecksum)
                {
                    printf("ERRO NO CHECKSUM\n");
                    nack->tipo = NACK;
                    ssize_t envio_nack = send(sock, nack, 132, 0);
                }*/

                printf("recebido %ld bytes\n", tam);
                printf("mensagem: %s\n", pack->dados);
                printf("tipo = %d\n", pack->tipo);
                
                //Teste
                //Captura a extensão do arquivo
                //char *extensao = devolve_extensao("foto_teste.jpg");
                //printf("Extensão do teste: %s\n", extensao);
                //exibe_arquivo("foto_teste.jpg");
                struct pacote *ack = ack_format_arq(pack);      //é um tipo de ack + ok
                printf("ack->tipo = %d\n", ack->tipo);

                ssize_t envio_ack = send(sock, ack, 132, 0);
                //se o ack não chegar tratar no timeout
            }
        }
        else
        {
            //A mensagem não chegou, então envia um nack
            nack->tipo = 1;
            ssize_t envio_nack = send(sock, nack, 132, 0);
        }
    }

    pclose(socket);
    return 0;
}