#ifndef _calibration_H
#define _calibration_H

#include <xc.h>

#define _XTAL_FREQ 64000000

void lightCalibration();
void motorCalibration(DC_motor *mL, DC_motor *mR);
void startBuggy(DC_motor *mL, DC_motor *mR);

#endif