#ifndef _tricolorLED_H
#define _tricolorLED_H

#include <xc.h>

#define _XTAL_FREQ 64000000

void initializeTriColorLED();
void turnOnWhiteLight();
void turnOnRedLight();
void turnOnGreenLight();
void turnOnBlueLight();

#endif