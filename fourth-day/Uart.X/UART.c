/*
 * File:   UART.c
 * Author: lucas
 *
 * Created on 4. Februar 2021, 09:25
 */


#include "Definitions.h"

char InitializeUart(const long int baudRate) {
    unsigned int x = (_XTAL_FREQ - baudRate * 64) / (baudRate * 64); //SPBRG for Low Baud Rate
    if (x > 255) //If High Baud Rage Required
    {
        // If PLL is enabled (HSPLL), XTAL_FREQ must be set (in Definitions.h)
        // 4 times higher than the Crystal actual frequency

        x = (_XTAL_FREQ - baudRate * 16) / (baudRate * 16); //SPBRG for High Baud Rate
        TXSTAbits.BRGH = 1; //Setting High Baud Rate
    }
    if (x < 256) {
        RCSTAbits.SPEN = 1; //Enables Serial Port
        RCSTAbits.CREN = 1; //Enables Continuous Reception

        TXSTAbits.SYNC = 0; //Setting Asynchronous Mode, ie UART
        TXSTAbits.TXEN = 1; //Enables Transmission

        SPBRG = x; //Writing SPBRG Register

        TRISCbits.RC7 = 1; //As Prescribed in Datasheet
        TRISCbits.RC6 = 1; //As Prescribed in Datasheet
        return 1; //Returns 1 to indicate Successful Completion
    }
    return 0;
}

/*Sending Data Through UART
Writing a Character*/
void WriteToUart(char data){
  while(!TRMT);
  TXREG = data;
}

//Checking Transmit Register
char IsUartTxEmpty(){
  return TRMT;
}

//Writing Text
void WriteTextToUart(char *text){
  int i;
  for(i=0;text[i]!='\0';i++)
    WriteToUart(text[i]);
}

/*Receiving Data Through UART
Data Received or Not
The following function can be used to check whether the data is ready to read from the Receive Register. 
It uses the flag bit RCIF which will be set when the data reception is completed.*/
char IsUartDataReady(){
  return PIR1bits.RCIF;
}

/*Reading a Character
The following function wait till the reception is complete and reads 8 bit data from the Receive Register.*/
char ReadUartData(){
  while(!PIR1bits.RCIF);
  return RCREG;
}

/*Reading Text
The following function can be used to read a desired length of text or sequence of characters continuously.*/
void ReadUartText(char *Output, unsigned int length){
  unsigned int i;
  for(int i=0;i<length;i++)
  Output[i] = ReadUartData();
}


