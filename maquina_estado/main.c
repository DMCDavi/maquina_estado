#include <msp430.h>

void config_msp() {
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer

    P1DIR |= BIT0;                  // configure P1.0 as output
    P2DIR |= BIT0;
    P1DIR |= 0x11;

    P1IE |= BIT3;    // Habilitando a interrupção p/ P1.3
    P1IES = 0x00;   // Definindo transição positiva

    P1SEL |= BIT4;                  // muda o P1.4 para SMCLK
    BCSCTL1 = CALBC1_1MHZ;          // configura o clock para 1MHz
    DCOCTL = CALDCO_1MHZ;
}


void main(void)
{
    config_msp();
    _enable_interrupts();
}


void led_turn_on() {
    P1OUT = 0x01;
}

void led_turn_off() {
    P1OUT = 0x00;
}

void led_blink() {
    int is_first_loop = 1, stop_blink = 0;

    while(!stop_blink){
        P1OUT ^= 0x01;

        volatile unsigned int i;
        for(i=500000; i>0; i--){
            if (!is_first_loop && (P1IN & BIT3) == BIT3){
                led_turn_off();
                stop_blink = 1;
            }
        }

        is_first_loop = 0;
    }
}

// Definição do vetor de interrupção
#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void){

    // se o botao for acionado o led eh aceso, senao apaga
    // (P1IN & BIT0) funciona como uma mascara para isolar apenas 1 bit
    if((P1IN & BIT0) == 0x00){
        led_turn_on();
    } else if((P1IN & BIT0) == BIT0){
        led_blink();
    }

    P1IFG = 0x00; //Limpando as flags de interrupção pra não ter problemas
}


