// timers.c
// Function to initialize the timer with prescaler and offset value

#include <xc.h>
#include "timers.h"

/************************************
 * Function to set up timer 0 so that it overflows after every 0.1 seconds
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4. Clock source setting bits
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b0101; // 1:32. 32 = 2 ** 5. Prescaler value bits as (62.5 * 10 ** -9) * (2 ** 5) * 65535 = 0.13107 seconds
    T0CON0bits.T016BIT=1;	//16bit mode	
	
    // Offset value
    // (0.03107 / ((62.5 * 10 ** -9) * (2 ** 5))) * 2 ** 5 = 20362 and 20362 is 0b0100111110001010 in binary
    TMR0H=0b01001111;            //write High reg first, update happens when low reg is written to
    TMR0L=0b10001010;
    T0CON0bits.T0EN=1;	//start the timer
}