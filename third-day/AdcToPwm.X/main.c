/*
 * File:   main.c
 * Author: lucas
 *
 * Created on 27. Februar 2021, 11:16
 */


#include "definitions.h"

void main(void) {
    
    // Set pin modes
    TRISAbits.RA0 = 1;
TRISCbits.RC2 = 0;

// Configure adc module
ADCON0bits.CHS = 0b0000;
ADCON0bits.GO  = 0;
ADCON0bits.ADON = 1;
ADCON1bits.VCFG = 0b00;
ADCON1bits.PCFG = 0b1110;
ADCON2bits.ADFM = 1;
ADCON2bits.ACQT = 0b010;
ADCON2bits.ADCS = 0b100;
    
// Configure timings and duty cycle
T2CONbits.T2CKPS  = 0b01;
T2CONbits.T2OUTPS = 0b0000;
T2CONbits.TMR2ON  = 1;
PR2 = 99;

// Enable pwm module
CCP1CONbits.CCP1M = 0b1100;

// Main program behaviour
while(1){    
    ADCON0bits.GO = 1;
    while(!ADCON0bits.GO);

    CCPR1L = ADRES / 16;
    CCP1CONbits.DC1B = 0b00;
}
    return;
}
