// interrupts.c
// Defines initialization function of timer interrupt and high priority interrupt service routine (ISR)

#include <xc.h>
#include "interrupts.h"

/************************************
 * Function to turn on timer interrupt and set priority
************************************/
void Interrupts_init(void)
{
	// turn on global interrupts, peripheral interrupts and the interrupt source 
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
    
    // This turns on the interrupt register associated with the comparator (comparator is CM1 so use C1)
//    PIE2bits.C1IE=1; 	//enable interrupt source for comparator
    PIE0bits.TMR0IE=1;      //enable timer interrupt
    
    
    // Set INTCONbits.GIEL & INTCONbits.IPEN for high and low priority respectively. 
    INTCONbits.GIEL=1; 
    INTCONbits.IPEN=1;
    // best practise is to set the global interrupt (turn all interrupts on) last
    INTCONbits.GIE=1; 	//turn on interrupts globally (when this is off, all interrupts are deactivated)
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR(void)
{
    if(PIR0bits.TMR0IF){ 					//check the interrupt source
        *pointertimerHasOverflowed = true;
        // (0.03107 / ((62.5 * 10 ** -9) * (2 ** 5))) * 2 ** 5 = 20362 and 20362 is 0b0100111110001010 in binary 
        TMR0H=0b01001111;            //write High reg first, update happens when low reg is written to
        TMR0L=0b10001010;
        PIR0bits.TMR0IF=0;				//clear the interrupt flag!
	}
}

