#include <msp430.h>
#define CONST_TACCR0 62499;

const int CONST_TIMEOUT = 10;
int state = 0;
int cycles = 0;

void init_timer(void) {
    TACTL = TASSEL_2 + ID_3 + MC_1;         // Resgistrador usado para configurar o timer A
                                            // MC_1: o modo de opera��o do timer ser� progressivo de 0 at� o valor armazenado em TACCR0
                                            // TASSEL_2: clock source do microcontrolador ser� MCLK
                                            // ID_3: divide o clock por 8

    TACCR0 = CONST_TACCR0;                  // Seta o valor utilizado para calcular o per�odo da onda

    TACCTL0 = CCIE;                         // Habilita a interrup��o para o timer A
}

void config_msp() {
    WDTCTL = WDTPW | WDTHOLD;               // Para o watchdog timer

    P1DIR |= BIT0;                          // Configura o pino P1.0 como sa�da

    P1IE |= BIT3;                           // Habilita a interrup��o para o pino P1.3 onde se encontra o bot�o
    P1IES = 0x00;                           // Define transi��o como positiva, ou seja, a interrup��o � acionada ao clicar no bot�o

    BCSCTL1 = CALBC1_1MHZ;                  // Configura o clock para 1MHz
    DCOCTL = CALDCO_1MHZ;                   // Configura o clock interno para 1MHz
}


void main(void)
{
    config_msp();
    init_timer();
    _enable_interrupts();                   // Habilita as interrup��es
}


void led_turn_off() {                       // Desliga o LED
    P1OUT = 0x00;
    state = 0;
    cycles = 0;
}

void led_turn_on() {                        // Liga o LED
    P1OUT = 0x01;
    state = 1;
    cycles = 0;
}

void led_blink(){                           // Pisca o LED
    if(state == 2){
       P1OUT ^= BIT0;
    }
}

void switch_blink_state() {                 // Muda o estado do LED para piscando
    state = 2;
    cycles = 0;
}

void verify_timeout() {                     // Desliga o LED de acordo ao timeout
    if (cycles == CONST_TIMEOUT) {
        led_turn_off();
    } else {
        cycles++;
    }
}

void switch_states() {                      // Muda os estados do LED
    if(state == 0){
        led_turn_on();
    } else if(state == 1){
        switch_blink_state();
    } else {
        led_turn_off();
    }
}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void){              // Implementa a interrup��o do bot�o
    switch_states();

    P1IFG = 0x00;                           // Limpa a flag de interrup��o
}

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A(void) {            // Implementa a interrup��o do timer A
    verify_timeout();
    led_blink();
}

