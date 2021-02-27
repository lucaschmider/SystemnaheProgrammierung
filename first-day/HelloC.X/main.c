/*
 * File:   mai.c
 * Author: lucas
 *
 * Created on 4. Februar 2021, 11:44
 */


#include "definitions.h"

void main(void) {
    LEDMode = 0;
    LED = 0;
    while(1) {
        LED = 1;
        __delay_ms(100);
        LED = 0;
        __delay_ms(900);
    }
    return;
}
