#ifndef _TIMER_H_
#define _TIMER_H_

// tempo do timeout em milisegundos
#define TIMEOUT_MAX 20 

// -------------------------------------------

// inicia temporizador
// a cada milisegundo ele dispara uma funcao que conta os milisegundos
// quando chega no timeout, reseta e muda a flag de timeout
void inicia_timer();

// retorna 1 se houve timeout
// retorna 0 se ainda nao
int deu_timeout();

// retorna o valor do tick atual
unsigned int timer();

#endif