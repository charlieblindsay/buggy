#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>
#include <stdbool.h>


#define _XTAL_FREQ 64000000

// global flag to signify that timer has overflowed
bool timerHasOverflowed;
bool *pointertimerHasOverflowed;

void Interrupts_init(void);
void __interrupt(high_priority) HighISR(void);



#endif
