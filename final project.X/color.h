#ifndef _color_H
#define _color_H

#include <xc.h>
#include <stdio.h>
#include <stdbool.h>

#include "dc_motor.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

// structure to contain the red, green, blue and clear ADC colour values from the colour sensor
typedef struct ADC_RGBC_ColourValues
{
    int red;
    int green;
    int blue;
    int clear;
} ADC_RGBC_ColourValues;

// global variable to store the ambient RGBC ADC colour values. 
// These are the reference values to which the current values are compared.
ADC_RGBC_ColourValues ambient_ADC_values;
ADC_RGBC_ColourValues *pointer_ambient_ADC_values;

// global variable to store the current RGBC ADC colour values
ADC_RGBC_ColourValues current_ADC_values;
ADC_RGBC_ColourValues *pointer_current_ADC_values;

// global variable to store the previous RGBC ADC colour values
ADC_RGBC_ColourValues previous_ADC_values;
ADC_RGBC_ColourValues *pointer_previous_ADC_values;

// global variable to signify whether the buggy has returned to the beginning
bool hasBlackCardBeenReached = false;
bool *pointerhasBlackCardBeenReached;

// Contains the threshold percentage increase of the red, green, blue and clear colour sensor
// values relative to ambient levels which signify when a certain coloured card is placed in 
// front of the colour sensor
typedef struct thresholdAbsolutePercentageChangeOf_ADC_ColourSensorValues{
    float red_ADC;
    float green_ADC;
    float blue_ADC;
    float clear_ADC;
} thresholdAbsolutePercentageChangeOf_ADC_ColourSensorValues;


// initialising the thresholdAbsolutePercentageChangeOf_ADC_ColourSensorValues objects for different coloured cards.

struct thresholdAbsolutePercentageChangeOf_ADC_ColourSensorValues whiteCardThresholds = {900,950,950,900}; //Minimum sensor reading for a white card
struct thresholdAbsolutePercentageChangeOf_ADC_ColourSensorValues blackCardThresholds = {120,120,120,120};  //Maxmimum sensor reading for a black card

struct thresholdAbsolutePercentageChangeOf_ADC_ColourSensorValues CardThreshold = {50,70,70,50};    //Minimum sensor variation to trigger card detection
struct thresholdAbsolutePercentageChangeOf_ADC_ColourSensorValues VariationThreshold = {30,30,30,30};   //Maximum variation in consecutive readings to trigger colour identification
struct thresholdAbsolutePercentageChangeOf_ADC_ColourSensorValues ColourToClearRatios = {50,50,50,0};  //Ratio to identify colour (see powerpoint)

//Ratios for light blue, pink, orange, yellow

int RC_ratio_orange = 15;
int BC_ratio_lblue = 40;
int GB_ratio_yellow = 15;
int GR_ratio_white = 30;



void colourDetection(DC_motor *mL, DC_motor *mR);

bool isCardDetected(void);

void color_click_init(void);
void color_writetoaddr(char address, char value);
int get_ADC_Colour_Value_by_address(unsigned int address);
ADC_RGBC_ColourValues get_ADC_RGBC_ColourValues();

#endif
