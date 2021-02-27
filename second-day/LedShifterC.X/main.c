/*
 * File:   main.c
 * Author: lucas
 *
 * Created on 2. Februar 2021, 09:09
 */


#include "Definitions.h"
void main(void) {
    TRISB = 0b00000000; // Set the direction for all pins in port b to output
    LATB = 0b0000001; // Set all pins except the first one to low
    volatile char shiftLeft = 1;
    while(1){
//        for(int i = 0; i < 7; i++) { // 
//            LATB = LATB << 1;
//            delayHalfSecond();
//        }
//        for(int i = 0; i < 7; i++) {
//            LATB = LATB >> 1;
//            delayHalfSecond();
//        }
        LATB = shiftLeft == 1 ? LATB << 1 : LATB >> 1;
        shiftLeft = (shiftLeft == 1 && LATB != 0b10000000) 
                || (shiftLeft == 0 && LATB == 0b00000001);
        __delay_ms(250);
         __delay_ms(250);
    }
    return;
}
