#include <msp430.h>
#define CONST_TACCR0 62499;

const int CONST_TIMEOUT = 10;
int state = 0;
int cycles = 0;

void init_timer(void) {
    TACTL = TASSEL_2 + ID_3 + MC_1;         // Resgistrador usado para configurar o timer A
                                            // MC_1: o modo de operação do timer será progressivo de 0 até o valor armazenado em TACCR0
                                            // TASSEL_2: clock source do microcontrolador será MCLK
                                            // ID_3: divide o clock por 8

    TACCR0 = CONST_TACCR0;                  // Seta o valor utilizado para calcular o período da onda

    TACCTL0 = CCIE;                         // Habilita a interrupção para o timer A
}

void config_msp() {
    WDTCTL = WDTPW | WDTHOLD;               // Para o watchdog timer

    P1DIR |= BIT0;                          // Configura o pino P1.0 como saída

    P1IE |= BIT3;                           // Habilita a interrupção para o pino P1.3 onde se encontra o botão
    P1IES = 0x00;                           // Define transição como positiva, ou seja, a interrupção é acionada ao clicar no botão

    BCSCTL1 = CALBC1_1MHZ;                  // Configura o clock para 1MHz
    DCOCTL = CALDCO_1MHZ;                   // Configura o clock interno para 1MHz
}


void main(void)
{
    config_msp();
    init_timer();
    _enable_interrupts();                   // Habilita as interrupções
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
__interrupt void Port_1(void){              // Implementa a interrupção do botão
    switch_states();

    P1IFG = 0x00;                           // Limpa a flag de interrupção
}

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A(void) {            // Implementa a interrupção do timer A
    verify_timeout();
    led_blink();
}

