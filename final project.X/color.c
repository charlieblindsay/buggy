// color.c
// Functions which use the colour sensor's ADC red, green, blue and clear colour values 

#include <xc.h>
#include <stdbool.h>
#include <stdio.h>

#include "color.h"

#include "dc_motor.h"
#include "i2c.h"
#include "serial.h"
#include "mathematics.h"


/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
}

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

// Function which uses the I2C interface to read the ADC colour value at the inputted address

// input: address
// returns: 16 bit ADC colour value
int get_ADC_Colour_Value_by_address(unsigned int address)
{    
	unsigned int tmp;  
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | address);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
    
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

// Function which calls above get_ADC_Colour_Value_by_address function for 
// the address of clear, red, green and blue and stores these in an instance
// of the struct ADC_RGBC_ColourValues

// inputs: none
// outputs: ADC_RGBC_ColourValues
ADC_RGBC_ColourValues get_ADC_RGBC_ColourValues(){
    struct ADC_RGBC_ColourValues ADC_RGBC_values;

    ADC_RGBC_values.clear = get_ADC_Colour_Value_by_address(0x14);
    ADC_RGBC_values.red = get_ADC_Colour_Value_by_address(0x16);
    ADC_RGBC_values.green = get_ADC_Colour_Value_by_address(0x18);
    ADC_RGBC_values.blue = get_ADC_Colour_Value_by_address(0x1A);

    return ADC_RGBC_values;
}

// Function which compares the absolute percentage change of the current 
// ADC colour values from ambient levels with the threshold change signifying
// the presence of a card. If the absolute percentage change exceeds the thresholds
// for all colour values, the function checks if the current reading varies significantly from the previous reading.
// If the ADC colour values have stablised, then the function will return "true", otherwise it will return "false".

// inputs: none

// output: boolean signifying whether or not a coloured card has been detected
bool isCardDetected(void){

    //Is the variation from ambient significant?

    if(absolutePercentageChange(ambient_ADC_values.red, current_ADC_values.red) > CardThreshold.red_ADC){
        if(absolutePercentageChange(ambient_ADC_values.green, current_ADC_values.green) > CardThreshold.green_ADC){
            if(absolutePercentageChange(ambient_ADC_values.blue, current_ADC_values.blue) > CardThreshold.blue_ADC){
                if(absolutePercentageChange(ambient_ADC_values.clear, current_ADC_values.clear) > CardThreshold.clear_ADC){

                    //Is the variation from the previous reading insignificant?
                    if(absolutePercentageChange(previous_ADC_values.red, current_ADC_values.red) < VariationThreshold.red_ADC){
                        if(absolutePercentageChange(previous_ADC_values.green, current_ADC_values.green) < VariationThreshold.green_ADC){
                            if(absolutePercentageChange(previous_ADC_values.blue, current_ADC_values.blue) < VariationThreshold.blue_ADC){
                                if(absolutePercentageChange(previous_ADC_values.clear, current_ADC_values.clear) < VariationThreshold.clear_ADC){
                                    *pointer_previous_ADC_values = current_ADC_values;
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    else{
        *pointer_previous_ADC_values = current_ADC_values;
        return false;
    }
    
}

// Function which implements the colour identifying algorithm
// If the function returns true for any coloured cards, the appropriate buggy turn is performed.

// Inputs: *mL: pointer to the left motor, *mR: pointer to the right motor

void colourDetection(DC_motor *mL, DC_motor *mR){

    if ((absolutePercentageChange(ambient_ADC_values.red, current_ADC_values.red) > whiteCardThresholds.red_ADC)&&(absolutePercentageChange(ambient_ADC_values.green, current_ADC_values.green) > whiteCardThresholds.green_ADC)&&(absolutePercentageChange(ambient_ADC_values.blue, current_ADC_values.blue) > whiteCardThresholds.blue_ADC)&&(absolutePercentageChange(ambient_ADC_values.clear, current_ADC_values.clear) > whiteCardThresholds.clear_ADC)){
        // Is the card orange?
        if (absolutePercentageChange(absolutePercentageChange(ambient_ADC_values.clear, current_ADC_values.clear),absolutePercentageChange(ambient_ADC_values.red, current_ADC_values.red))>RC_ratio_orange){
            performBuggyActionAccordingToCard(mL, mR, 135, false);
        }
        // Is the card yellow?
        else if ((absolutePercentageChange(absolutePercentageChange(ambient_ADC_values.blue, current_ADC_values.blue),absolutePercentageChange(ambient_ADC_values.green, current_ADC_values.green))>GB_ratio_yellow)&&(absolutePercentageChange(ambient_ADC_values.green, current_ADC_values.green)>absolutePercentageChange(ambient_ADC_values.blue, current_ADC_values.blue))){
            performBuggyActionAccordingToCard(mL, mR, 90, true);
        }  
        // Is the card white?
        else if (absolutePercentageChange(absolutePercentageChange(ambient_ADC_values.red, current_ADC_values.red),absolutePercentageChange(ambient_ADC_values.green, current_ADC_values.green))>GR_ratio_white){
            performBuggyActionAccordingToCard(mL, mR, 180, false);
        }

        // The card is pink
        else {
            performBuggyActionAccordingToCard(mL, mR, 270, true);
        }
        }

    //Checking if the card is light blue
    else if ((absolutePercentageChange(ambient_ADC_values.red, current_ADC_values.red) < whiteCardThresholds.red_ADC)&&(absolutePercentageChange(ambient_ADC_values.green, current_ADC_values.green) > whiteCardThresholds.green_ADC)&&(absolutePercentageChange(ambient_ADC_values.blue, current_ADC_values.blue) > whiteCardThresholds.blue_ADC)&&(absolutePercentageChange(ambient_ADC_values.clear, current_ADC_values.clear) > whiteCardThresholds.clear_ADC)){
        if (absolutePercentageChange(absolutePercentageChange(ambient_ADC_values.clear, current_ADC_values.clear),absolutePercentageChange(ambient_ADC_values.blue, current_ADC_values.blue))>BC_ratio_lblue){
            performBuggyActionAccordingToCard(mL, mR, 225, false);
        }
    }

    //Checking if card is black
    else if((absolutePercentageChange(ambient_ADC_values.red, current_ADC_values.red) < blackCardThresholds.red_ADC)&&(absolutePercentageChange(ambient_ADC_values.green, current_ADC_values.green) < blackCardThresholds.green_ADC)&&(absolutePercentageChange(ambient_ADC_values.blue, current_ADC_values.blue) < blackCardThresholds.blue_ADC)&&(absolutePercentageChange(ambient_ADC_values.clear, current_ADC_values.clear) < blackCardThresholds.clear_ADC)){
        stopBuggy(mL,mR);

    }

    else{
        //Is the card red?
        if((absolutePercentageChange(ambient_ADC_values.red, current_ADC_values.red)>absolutePercentageChange(ambient_ADC_values.blue, current_ADC_values.blue))&&(absolutePercentageChange(ambient_ADC_values.red, current_ADC_values.red)>absolutePercentageChange(ambient_ADC_values.green, current_ADC_values.green))){
            
        if(absolutePercentageChange(absolutePercentageChange(ambient_ADC_values.clear, current_ADC_values.clear),absolutePercentageChange(ambient_ADC_values.red, current_ADC_values.red))>ColourToClearRatios.red_ADC){
            performBuggyActionAccordingToCard(mL, mR, 90, false);    
            }
        }

        //Is the card blue?
        else if((absolutePercentageChange(ambient_ADC_values.blue, current_ADC_values.blue)>absolutePercentageChange(ambient_ADC_values.red, current_ADC_values.red))&&(absolutePercentageChange(ambient_ADC_values.blue, current_ADC_values.blue)>absolutePercentageChange(ambient_ADC_values.green, current_ADC_values.green))){
        
        if(absolutePercentageChange(absolutePercentageChange(ambient_ADC_values.clear, current_ADC_values.clear),absolutePercentageChange(ambient_ADC_values.blue, current_ADC_values.blue))>ColourToClearRatios.blue_ADC){
            performBuggyActionAccordingToCard(mL, mR, 180, false);
        }
        }

        //Is the card green?
        else if((absolutePercentageChange(ambient_ADC_values.green, current_ADC_values.green)>absolutePercentageChange(ambient_ADC_values.blue, current_ADC_values.blue))&&(absolutePercentageChange(ambient_ADC_values.green, current_ADC_values.green)>absolutePercentageChange(ambient_ADC_values.red, current_ADC_values.red))){
        
        if(absolutePercentageChange(absolutePercentageChange(ambient_ADC_values.clear, current_ADC_values.clear),absolutePercentageChange(ambient_ADC_values.green, current_ADC_values.green))>ColourToClearRatios.green_ADC){
            performBuggyActionAccordingToCard(mL, mR, 270, false);
        }
        }
    }
}