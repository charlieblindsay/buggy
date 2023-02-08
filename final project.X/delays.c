// delays.c

#include <xc.h>
#include "delays.h"

// function to create delays which are longer than 1 second (used in callibration.c)

// input: number of milliseconds in delay
delay_ms(unsigned int milliseconds)
 {
   while(milliseconds > 0)
   {
       __delay_ms(1);
      milliseconds--;
    }
 }