/*
 * File:   main.c
 * Author: lucas
 *
 * Created on 3. Februar 2021, 14:23
 */


#include "definitions.h"

void main(void) {
    T1CONbits.RD16 = 1; // Enables register read/write of TImer1 in one 16-bit operation
    T1CONbits.T1RUN = 1; // Device clock is derived from Timer1 oscillator
    T1CONbits.T1CKPS = 0b00; // 1:1 Prescaler 
    T1CONbits.T1OSCEN = 1; // Timer1 oscillator is enabled
    T1CONbits.T1SYNC = 1;
    T1CONbits.TMR1CS = 1; // External clock from pin RC0/T1OSO/T13CKI (on the rising edge)
    T1CONbits.TMR1ON = 1; // Start TMR1


    LEDMode = 0; // Make RB0 Output
    TMR1 = 0x8000; // Initialize TMR1 for a 1s Tick

    while (1) {
        Nop();
        if (TimerOverflowed) { // ? Timer1 Flagged Overflow
            TimerOverflowed = 0; // clear interrupt flag
            TMR1 = 0x8000; // again 385 via fine tuning
            LED = !LED; // toggle RB0
            Nop();
        }
    }
    return;
}
