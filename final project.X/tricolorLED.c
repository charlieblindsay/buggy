// tricolorLED.c
// Functions for controlling the tricolour LED

#include <xc.h>
#include "color.h"
#include "i2c.h"

// function which initializes the pins of the tricolour LEDs as outputs

// input: none
// output: none
void initializeTriColorLED(){
    TRISGbits.TRISG1 = 0; // making red pin output
    TRISAbits.TRISA4 = 0; // making blue pin output
    TRISFbits.TRISF7 = 0; // making green pin output
} 

// function which turns on all 3 colours of tricolour LED making white
// input: none
// output: none
void turnOnWhiteLight(){
    LATGbits.LATG1 = 1; //R
    LATAbits.LATA4 = 1; //G
    LATFbits.LATF7 = 1; //B
}

// function which turns on red light of tricolour LED
// input: none
// output: none
void turnOnRedLight(){
    LATGbits.LATG1 = 1;
    LATAbits.LATA4 = 0;
    LATFbits.LATF7 = 0;
}

// function which turns on green light of tricolour LED
// input: none
// output: none
void turnOnGreenLight(){
    LATGbits.LATG1 = 0;
    LATAbits.LATA4 = 1;
    LATFbits.LATF7 = 0;
}

// function which turns on blue light of tricolour LED
// input: none
// output: none
void turnOnBlueLight(){
    LATGbits.LATG1 = 0;
    LATAbits.LATA4 = 0;
    LATFbits.LATF7 = 1;
}