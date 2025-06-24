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

    limpa_terminal();

    // espera conexao do cliente
    while (espera_pacote(sock, pack_send, pack_recv) != SYN) {
        printf("esperando syn\n");
    };

    // cria tabuleiro para jogo
    struct tabuleiro_t *tabuleiro = cria_tabuleiro();

    while (procurando_tesouros_tabuleiro(tabuleiro)) {
        // imprime informacoes do jogo
        infos_jogo(tabuleiro);
        printf("Posicao jogador: [%d][%d]\n", tabuleiro->posicao_jogador.x, tabuleiro->posicao_jogador.y);

        uint8_t tipo = espera_pacote(sock, pack_send, pack_recv);
        // espera receber pacote de movimento
        uint8_t movimento_possivel = 0;
        switch (tipo)
        {
            case CIMA:
                movimento_possivel = movimenta_jogador(tabuleiro, MOVE_CIMA);
                break;
            case BAIXO:
                movimento_possivel = movimenta_jogador(tabuleiro, MOVE_BAIXO);
                break;
            case DIR:
                movimento_possivel = movimenta_jogador(tabuleiro, MOVE_DIR);
                break;
            case ESQ:
                movimento_possivel = movimenta_jogador(tabuleiro, MOVE_ESQ);
                break;
            
            default:
                break;
        }
        uint8_t msg; // mensagem adicional

        if (movimento_possivel) {
            // descobre se encontrou tesouro na posicao
            int tesouro = encontrou_tesouro(tabuleiro); // devolve numero do tesouro
            if (tesouro >= 0) {
                // achou tesouro
                msg = TESOURO;
                escreve_pacote(pack_send, OK, 1, 0, &msg);
                envia_pacote(sock, pack_send);
                espera_ack(sock, pack_send, pack_recv);
                // vai enviar arquivo
                envia_dados(sock, pack_send, pack_recv, nome_tesouro(tesouro));
            }
            else {
                // nao encontrou tesouro
                msg = NORMAL; // posicao normal
                escreve_pacote(pack_send, OK, 1, 0, &msg);
                espera_ack(sock, pack_send, pack_recv);
            }
        }
        else {
            // movimento invalido
            msg = MOVE_INV;
            escreve_pacote(pack_send, ERRO, 1, 0, &msg);
            envia_pacote(sock, pack_send);
            espera_ack(sock, pack_send, pack_recv);
        }

    }


    destroi_tabuleiro(tabuleiro);
    destroi_pacote(pack_recv);
    destroi_pacote(pack_send);
    close(sock);

    return 0;
}