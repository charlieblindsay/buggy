// calibration.c
// Functions used in calibration sequence which user performs before buggy attempts maze.
// See README4INSTRUCTIONS.md for a flow chart

#include <xc.h>
#include "dc_motor.h"
#include "tricolorLED.h"
#include "color.h"
#include "calibration.h"

// function which sets the ambient light levels.
// It sets global variable *pointer_ambient_ADC_values to be equal to the current ADC RGBC
// values when user presses RF2.

// input: none
// output: none
void lightCalibration(){
    while (PORTFbits.RF2); // empty while loop to wait for button press to set ambient light conditions
    
    if (!PORTFbits.RF2) {
        *pointer_ambient_ADC_values = get_ADC_RGBC_ColourValues();
    }
}

// function which spins the buggy 360 degrees and allows the user to press RF2 and RF3 to adjust the
// turnDuration global variable. The user can then either choose to repeat the spin or continue when
// happy with the adjustments

// inputs: 
// - *mL: pointer to left motor
// - *mR: pointer to right motor
// output: none
void motorCalibration(DC_motor *mL, DC_motor *mR){    
        turnOnBlueLight();
        // While either button is not pressed:
        // When both buttons pressed, while loop exited 
        delay_ms(3000);
        // This while loop means that the turn delay can be increased or decreased by more than one increment
        while (PORTFbits.RF2 || PORTFbits.RF3){
            
            // - When button RF2 (right button pressed)
            if (!PORTFbits.RF2) {
                *pointerturnDuration += 2; // turn delay increased (for when buggy underturned during square drive)
                turnOnGreenLight();
                delay_ms(1000);
                turnOnBlueLight();
            }

            // - When button RF3 (left button pressed)
            if (!PORTFbits.RF3) {
                *pointerturnDuration -= 2; // turn delay reduced (for when buggy overturned during square drive)
                turnOnRedLight();
                delay_ms(1000);
                turnOnBlueLight();
            }
            delay_ms(1000);
        }
        
    turnOnRedLight();
    delay_ms(3000);
    
    while (PORTFbits.RF3); // empty while loop to wait for button press
        
            // Code below makes buggy drive in a square. This is to see if the buggy is overturning or underturning
    if (!PORTFbits.RF3) {
            turnOnGreenLight();
                for(int i=0; i<4; i++){
                    turnBuggy(mL, mR, 270);
                }
    }

}

// function to set buggy loose on the maze. 
// It sets the buggy in forward motion and turns on the white light after the user presses the RF3 button

// inputs: 
// - *mL: pointer to left motor
// - *mR: pointer to right motor
// output: none
void startBuggy(DC_motor *mL, DC_motor *mR){
    while (PORTFbits.RF3); // empty while loop to wait for button press to start buggy and turn on white light
    
    if (!PORTFbits.RF3) {
        turnOnWhiteLight();
        fullSpeedAhead(mL, mR, 15);
    }

}