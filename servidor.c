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
                
                printf("recebido %ld bytes\n", tam);
                printf("mensagem: %s\n", pack->dados);
                printf("tipo = %d\n", pack->tipo);

                //Captura a extensão do arquivo
                //char *extensao = devolve_extensao("foto_teste.jpg");
                //printf("Extensão do teste: %s\n", extensao);
            }
        }
    }

    pclose(socket);
    return 0;
}