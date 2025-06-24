#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
#include "timer.h"


unsigned int ticks = 0; // relogio do timeout em milisegundos
int timeout = 0; // flag para timeout

// --------------------------------------------------------------------


// funcao de tratamento de ticks de relogio
// chamada a cada 1ms
void handler_ticks() {
    // incrementa tempo do timeout e verifica se ja esgotou o tempo
    if (++ticks > TIMEOUT_MAX) {
        // se sim, reseta
        ticks = 0;
        timeout = 1;
    }
    else {
        timeout = 0;
    }
}


void inicia_timer() {

    struct sigaction action; // estrutura para tratar sinais
    struct itimerval timer; // estrutura para tratar o temporizador

    // configuracao do temporizador
    action.sa_handler = handler_ticks;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGALRM, &action, 0) < 0) {
        perror("Erro em sigaction");
        exit(1);
    }

    timer.it_value.tv_usec = 1000;    // 1ms para primeiro disparo
    timer.it_value.tv_sec = 0;
    timer.it_interval.tv_usec = 1000; // 1ms para disparos subsequentes
    timer.it_interval.tv_sec = 0;

    if (setitimer(ITIMER_REAL, &timer, 0) < 0) {
        perror("erro em setitimer");
        exit(1);
    }

    return;
}


int deu_timeout() {
    return (timeout);
}

unsigned int timer() {
    return (ticks);
}