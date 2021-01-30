/*
 * File:   main.c
 * Author: lucas
 *
 * Created on 2. Februar 2021, 10:52
 */


#include "Definitions.h"

void main(void) {
    ButtonMode = 1;
    LEDMode = 0;
    LATB = 0;
    char previousState = 0;
    while(1) {
        char currentState = Button;
        if (currentState == 0 && previousState != currentState) {
            LED = !LED;
            __delay_ms(20);
        }
        previousState = currentState; // Zu beachten: vorher jedes mal einlegen, gab fehler => so schnell, dass daten nicht zueinander passen
    }
    
    return;
}
