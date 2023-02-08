// serial.c
// Functions used to write data to the serial monitor

#include <xc.h>
#include "serial.h"

//function to set up USART4 for Reception and Transmission =
void initUSART4(void) {


    BAUD4CONbits.BRG16 = 0; 	//set baud rate scaling
    TX4STAbits.BRGH = 0; 		//high baud rate select bit
    SP4BRGL = 103; 			    //set baud rate to 103 = 9600bps
    SP4BRGH = 0;			    //not used

    RC4STAbits.CREN = 1; 		//enable continous reception
    TX4STAbits.TXEN = 1; 		//enable transmitter
    RC4STAbits.SPEN = 1; 		//enable serial port
    
    RC0PPS = 0x12; // Map EUSART4 TX to RC0
    RX4PPS = 0x11; // RX is RC1  
    
    TRISCbits.TRISC1 = 1;
}

//function to check the TX reg is free and send a byte
void sendCharSerial4(char charToSend) {
    while (!PIR4bits.TX4IF); // wait for flag to be set
    TX4REG = charToSend; //transfer char to transmitter
}

//function to send a string over the serial interface
void sendStringSerial4(char *string){
    while(*string != 0){
		sendCharSerial4(*string++); 	//Send out the current byte pointed to and increment the pointer
	}
}