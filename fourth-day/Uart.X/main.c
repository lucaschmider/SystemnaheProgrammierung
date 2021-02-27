/*
 * File:   main.c
 * Author: lucas
 *
 * Created on 4. Februar 2021, 09:18
 */


#include "Definitions.h"
#include <stdio.h>

void main(void) {
    const char initilizationResult = InitializeUart(9600);
    volatile char received_char;
    if (initilizationResult == 0) {
        //  Handle Error
        while(1);
    }
    
    WriteTextToUart("Hello, World");
    while(1){
      if(IsUartDataReady()){       // wait for successful char reception
        received_char = ReadUartData();
        PIR1bits.RCIF = 0;         // clear reception flag
        __delay_ms(10);            // delay
        LATD = received_char;      // display the char on 7 segment LEDs
        WriteToUart(received_char); // echo received char
        }
    } 
}

void putch(unsigned char data) {
 while(!PIR1bits.TXIF)   // wait until the transmitter is ready
        continue;
 TXREG = data;           // send one character
}