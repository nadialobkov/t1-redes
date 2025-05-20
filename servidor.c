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
    printf("esperando envio do caminho do arquivo...\n");

    // vamos fazer um loop para ficar esperando mensagens
    while (1) {
        // recebe dados
        ssize_t tam = recv(sock, pack, 132, 0);

        if (tam > 0) {
            // verifica marcador de inicio
            if (pack->marcador == 0x7e) {
                //alterei um bit para testar se a função encontra o erro
                //pack->dados[0] = pack->dados[0] ^ 0xFF;

                int verificaChecksum = verifica_checksum(pack);
                if (!verificaChecksum)
                    printf("ERRO NO CHECKSUM\n");
                

                char *caminho = pack->dados; 
                int tam = strlen(caminho);
                caminho[tam-1] = 0; // tira \n
                printf("recebido: %s\n", caminho);


                struct pacote **packets = prepara_pacotes_dados((const char *) pack->dados);
                if (packets ==  NULL) {
                    printf("erro em criar pacotes\n");

                }

                ssize_t i = 0;
                printf("oi\n");
                printf("%s\n", packets[i]->dados);
                while (packets[i]->tipo == DADOS) { // nao sei se esta muito seguro
                    printf("enviando pacote %d", i);
                    send(sock, packets[i], 131, 0);
                    i++;
                }
                pack->tipo = FIM;
                send(sock, pack, 131, 0);
            }
        }
    }

    pclose(socket);
    return 0;
}