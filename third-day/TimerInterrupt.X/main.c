/*
 * File:   main.c
 * Author: lucas
 *
 * Created on 4. Februar 2021, 14:33
 */


#include "definitions.h"

void __interrupt(high_priority) TMR1Int(void) {
    PIR1bits.TMR1IF = 0;
    TMR1 = 0x8000;
    TimerLed = !TimerLed;
    return;
}

void main(void) {
    T1CONbits.RD16 = 1; // Enables register read/write of TImer1 in one 16-bit operation
    T1CONbits.T1RUN = 1; // Device clock is derived from Timer1 oscillator
    T1CONbits.T1CKPS = 0b00; // 1:1 Prescaler 
    T1CONbits.T1OSCEN = 1; // Timer1 oscillator is enabled
    T1CONbits.T1SYNC = 1;
    T1CONbits.TMR1CS = 1; // External clock from pin RC0/T1OSO/T13CKI (on the rising edge)
    T1CONbits.TMR1ON = 1; // Start TMR1


    TimerLedMode = 0; // Make RB0 Output
    RandomLedMode = 0;
    TMR1 = 0x8000; // Initialize TMR1 for a 1s Tick

    PIE1bits.TMR1IE = 1; // enable TMR1 Interrupts (TMR1IE)
    INTCONbits.GIE = 1; // enable global interrupts (GIE)
    INTCONbits.PEIE = 1; // enable peripheral interrupts (PEIE)

    while (1) {
        RandomLed = !RandomLed;
        __delay_ms(500);
    }
    return;
}
