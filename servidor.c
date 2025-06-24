#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>

#include "sock.h"
#include "pacote.h"
#include "timer.h"
#include "jogo.h"

int main() {

    // criamos um socket com interface das maquinas virtuais 
    int sock = cria_raw_socket("veth1");

    pacote_t *pack_send = cria_pacote();
    pacote_t *pack_recv = cria_pacote();

    inicia_timer();

    // espera conexao do cliente
    while (espera_pacote(sock, pack_send, pack_recv) != SYN);

    // cria tabuleiro para jogo
    struct tabuleiro_t *tabuleiro = cria_tabuleiro();
    // imprime informacoes do jogo
    infos_jogo(tabuleiro);

    
    
    envia_dados(sock, pack_send, pack_recv, "medio.txt");

    destroi_tabuleiro(tabuleiro);
    destroi_pacote(pack_recv);
    destroi_pacote(pack_send);
    close(sock);

    return 0;
}