/*
 * File:   main.c
 * Author: lucas
 *
 * Created on 27. Februar 2021, 11:08
 */


#include "definitions.h"

void main(void) {
    TRISCbits.RC2 = 0;
    T2CONbits.T2CKPS  = 0b01;
    T2CONbits.T2OUTPS = 0b0000;
    T2CONbits.TMR2ON  = 1;
    PR2 = 99;
    CCP1CONbits.CCP1M = 0b1100;

    CCPR1L = 0b00101000;
    CCP1CONbits.DC1B = 0b00;

    while(1);
    return;
}
