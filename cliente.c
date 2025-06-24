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
    int sock = cria_raw_socket("veth0");
  
    // cria pacote para enviar mensagens
    pacote_t *pack_send = cria_pacote();
    pacote_t *pack_recv = cria_pacote();

    inicia_timer(); 

    // cria jogador
    struct jogador_t *jogador = cria_jogador();

    limpa_terminal();
    imprime_mapa(jogador);

    // inicia conexao enviando um pacote para sincronizar com o servidor
    escreve_pacote(pack_send, SYN, 0, 0, NULL);
    envia_pacote(sock, pack_send);
    espera_ack(sock, pack_send, pack_recv);



    while (procurando_tesouros(jogador)) {

        // printf("Posicao jogador: [%d][%d]\n", jogador->pos_x, jogador->pos_y);

        uint8_t move = le_movimento();
        // escreve pacote com base no movimento
        switch (move) {
            case MOVE_CIMA:
                escreve_pacote(pack_send, CIMA, 0, 0, NULL);
                break;

            case MOVE_BAIXO:
                escreve_pacote(pack_send, BAIXO, 0, 0, NULL);
                break;

            case MOVE_DIR:
                escreve_pacote(pack_send, DIR, 0, 0, NULL);
                break;
                
            case MOVE_ESQ:
                escreve_pacote(pack_send, ESQ, 0, 0, NULL);
                break;
            
            default:
                // tecla invalida, repete while
                continue;
                break;
        }
        envia_pacote(sock, pack_send);
        espera_ack(sock, pack_send, pack_recv);
        uint8_t tipo = espera_pacote(sock, pack_send, pack_recv);

        // trata pacote recebido
        
        uint8_t msg;

        if (tipo == OK) {
            msg = dado_pacote(pack_recv);
            if (msg == NORMAL) {
                atualiza_jogador(jogador, move, SEM_TESOURO_VISITADA);
            }
            else if (msg == TESOURO) {
                atualiza_jogador(jogador, move, COM_TESOURO_VISITADA);
                recebe_dados(sock, pack_send, pack_recv);
            }
            limpa_terminal();
            imprime_mapa(jogador);
        }
        else if (tipo == ERRO) {
            msg = dado_pacote(pack_recv);
            if (msg == MOVE_INV) {
                printf("Bateu na parede! 🤕");
            }
        }
        // limpa_terminal();
        
    }

    mensagem_vitoria(jogador);


    destroi_jogador(jogador);
    destroi_pacote(pack_send);
    destroi_pacote(pack_recv);
    close(sock);

    return 0;
}