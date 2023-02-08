/************************************
 * #pragma directives...
************************************/
#pragma config FEXTOSC = HS
#pragma config RSTOSC = EXTOSC_4PLL 
#pragma config WDTE = OFF        

/************************************
 * #define directives...
 ************************************/
#define _XTAL_FREQ 64000000 

#include <xc.h>
#include <stdio.h>
#include <stdbool.h>

#include "i2c.h"
#include "color.h"
#include "timers.h"
#include "interrupts.h"
#include "serial.h"
#include "dc_motor.h"
#include "tricolorLED.h"
#include "delays.h"
#include "calibration.h"
#include "delays.h"
#include "mathematics.h"
#include "inputButtons.h"


void main(void) {

    // Setting address of pointers from dc_motor.h
    pointerforwardSequenceDistanceCounter = &forwardSequenceDistanceCounter;
    pointerreverseSequenceDistanceCounter = &reverseSequenceDistanceCounter;
    pointerturnDuration = &turnDuration;
    
    // Setting address of pointers from color.h
    pointer_ambient_ADC_values = &ambient_ADC_values;
    pointer_current_ADC_values = &current_ADC_values;
    pointerhasBlackCardBeenReached = &hasBlackCardBeenReached;

    // Setting addresses of pointers from interrupts.h
    pointertimerHasOverflowed = &timerHasOverflowed;

    // Component Initialisations
    color_click_init();
    I2C_2_Master_Init();
    initUSART4();
    Interrupts_init();
    Timer0_init();
    initDCmotorsPWM();
    initializeTriColorLED();
    initializeInputButtons();

    // initialises both left and right motors to have 0 power, forward direction and slow decay brakemode
    DC_motor mL = init_DC_motor(true); 
    DC_motor mR = init_DC_motor(false);
    
    // sets PWM output for left and right motors given their initial power, direction and brakemode
    setMotorPWM(&mL);
    setMotorPWM(&mR);
    
    *pointer_previous_ADC_values = get_ADC_RGBC_ColourValues(); //

    // LIGHT CALIBRATION
    lightCalibration();
   
    // BUGGY TURN CALIBRATION
     motorCalibration(&mL, &mR);

    // // MOVE FORWARDS AND TURN ON WHITE LIGHT
     startBuggy(&mL, &mR);

    while(*pointerhasBlackCardBeenReached == false){
        // CHECKING WHETHER CARD IS DETECTED
        if(*pointertimerHasOverflowed){ // timer overflows every 0.1 seconds which sets this variable to true 
            *pointer_current_ADC_values = get_ADC_RGBC_ColourValues();

            // More computationally expensive colour detection only performed if a card is detected
            if(isCardDetected()){ // simple check to determine if any card is present
               colourDetection(&mL, &mR); // does more complex (and computationally expensive) check to determine which card is present
            }
            *pointertimerHasOverflowed = false;
        }
    }
}