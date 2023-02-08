// dc_motor.c
// Functions which use the DC motors on the buggy

#include <xc.h>
#include <stdbool.h>

#include "dc_motor.h"
#include "color.h"
#include "delays.h"

// Function to initialize a DC_motor object

//inputs: isLeftMotorBool
//output: DC_motor object
DC_motor init_DC_motor(bool isLeftMotorBool){
    struct DC_motor motor; 		//declare two DC_motor structures 

    motor.power=0; 						//zero power to start
    motor.direction=1; 					//set default motor direction
    motor.brakemode=1;						// brake mode (slow decay)
    
    if(isLeftMotorBool){
        motor.posDutyHighByte=(unsigned char *)(&CCPR1H);  //store address of CCP1 duty high byte
        motor.negDutyHighByte=(unsigned char *)(&CCPR2H);  //store address of CCP2 duty high byte
    }
    else{
        motor.posDutyHighByte=(unsigned char *)(&CCPR3H);  //store address of CCP3 duty high byte
        motor.negDutyHighByte=(unsigned char *)(&CCPR4H);  //store address of CCP4 duty high byte
    }

    motor.PWMperiod=T2PR; 			//store PWMperiod for motor (value of T2PR in this case)
    
    return motor;
    //same for motorR but different CCP registers
}

// function initialise T2 and CCP for DC motor control

// input: none
// output: none
void initDCmotorsPWM(){
    //initialise your TRIS and LAT registers for PWM  
    TRISEbits.TRISE2 = 0;
    TRISEbits.TRISE4 = 0;
    TRISCbits.TRISC7 = 0;
    TRISGbits.TRISG6 = 0;
    
    //configure PPS to map CCP modules to pins
    RE2PPS=0x05; //CCP1 on RE2
    RE4PPS=0x06; //CCP2 on RE4
    RC7PPS=0x07; //CCP3 on RC7
    RG6PPS=0x08; //CCP4 on RG6

    // timer 2 config

    // prescaler = (64 * 10 ** 6 * 10 ** (-4)) / 4 * 255 = 6.27 (therefore 2 ** 3 = 8) 
    T2CONbits.CKPS=0b0011; // 1:3 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // T2PR = ((10 ** (-4)) * (64 * 10 ** 6)) / 8 - 1 = 199
    T2PR=199; //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H=0; 
    CCPR2H=0; 
    CCPR3H=0; 
    CCPR4H=0; 
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL=0;
    CCPTMRS0bits.C2TSEL=0;
    CCPTMRS0bits.C3TSEL=0;
    CCPTMRS0bits.C4TSEL=0;
    
    //configure each CCP
    CCP1CONbits.FMT=1; // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE=0b1100; //PWM mode  
    CCP1CONbits.EN=1; //turn on
    
    CCP2CONbits.FMT=1; // left aligned
    CCP2CONbits.CCP2MODE=0b1100; //PWM mode  
    CCP2CONbits.EN=1; //turn on
    
    CCP3CONbits.FMT=1; // left aligned
    CCP3CONbits.CCP3MODE=0b1100; //PWM mode  
    CCP3CONbits.EN=1; //turn on
    
    CCP4CONbits.FMT=1; // left aligned
    CCP4CONbits.CCP4MODE=0b1100; //PWM mode  
    CCP4CONbits.EN=1; //turn on
}

// function to set CCP PWM output from the values in the motor structure

// input: pointer to motor object
// output: none
void setMotorPWM(DC_motor *m)
{
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor
    
    if(m->brakemode) {
        posDuty=m->PWMperiod - ((unsigned int)(m->power)*(m->PWMperiod))/100; //inverted PWM duty
        negDuty=m->PWMperiod; //other side of motor is high all the time
    }
    else {
        posDuty=((unsigned int)(m->power)*(m->PWMperiod))/100; // PWM duty
        negDuty=0; //other side of motor is low all the time
    }

    // when direction channel is high, the power is controlled by the PWM off period
    // (i.e. 75% duty cycle gives 25% power)
    if (m->direction) {
        *(m->posDutyHighByte)=posDuty;  //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte)=negDuty;

    // when direction channel is low, the power is controlled by the PWM on period
    // (i.e. 25% duty cycle gives 25% power)
    } else {
        *(m->posDutyHighByte)=negDuty;  //do it the other way around to change direction
        *(m->negDutyHighByte)=posDuty;
    }
}

// Function which stops the buggy

// Inputs:
// *mL: pointer to the left motor
// *mR: pointer to the right motor

// output: none
void stopBuggy(DC_motor *mL, DC_motor *mR)
{
    
    (*mL).power = 0;
    (*mR).power = 0;
    
    setMotorPWM(mL);
    setMotorPWM(mR);
}


// Function which turns the buggy
// Takes turnAngleInDegrees as input. If this angle is less than 180 degrees, the buggy turns left by this angle. 
// If the angle is greater than 180 degrees, it turns right by an angle (360-turnAngleInDegrees) 

// Inputs:
// *mL: pointer to the left motor
// *mR: pointer to the right motor
// clockwiseTurnAngleInDegrees: the number of degrees to turn by in a clockwise direction

// output: none
void turnBuggy(DC_motor *mL, DC_motor *mR, unsigned int clockwiseTurnAngleInDegrees)
{
    stopBuggy(mL, mR);
    __delay_ms(1000);

    int turnAngleInDegrees;
    
    if(clockwiseTurnAngleInDegrees > 180){ // left turn
        (*mL).direction = 0;
        (*mR).direction = 1;
        turnAngleInDegrees = 360 - clockwiseTurnAngleInDegrees;
    }
    else{ // right turn
        (*mL).direction = 1;
        (*mR).direction = 0;
        turnAngleInDegrees = clockwiseTurnAngleInDegrees;
    }

    setMotorPWM(mL);
    setMotorPWM(mR);

    // buggy will turn in 15 degree increments (i.e. in bursts)
    int numberOfFifteenDegreeIncrements = turnAngleInDegrees / 15;

    for(int i = 0; i < numberOfFifteenDegreeIncrements; i++){
        fullSpeedAhead(mL, mR, motorPower);
        delay_ms(*pointerturnDuration);
        stopBuggy(mL, mR);
        __delay_ms(200);
    }

    
    if(clockwiseTurnAngleInDegrees > 180){ // left turn
        (*mL).direction = 1;
        setMotorPWM(mL);
    }
    else{
        (*mR).direction = 1;
        setMotorPWM(mR);
    }


}

// Function which makes the buggy go straight ahead

// Inputs:
// *mL: pointer to the left motor
// *mR: pointer to the right motor
// power: power given to both left and right motors (determines motor speed)

// output: none
void fullSpeedAhead(DC_motor *mL, DC_motor *mR, char power)
{
    (*mL).power = power;
    (*mR).power = power;

    setMotorPWM(mL);
    setMotorPWM(mR);
}


// Function which makes the buggy reverse

// Inputs:
// *mL: pointer to the left motor
// *mR: pointer to the right motor
// power: power given to both left and right motors (determines motor speed)
// duration: time delay in ms added to the reverse

// output: none
void reverseBuggy(DC_motor *mL, DC_motor *mR, unsigned int power, unsigned int duration){
    (*mL).direction = 0;
    (*mR).direction = 0;

    (*mL).power = power;
    (*mR).power = power;

    setMotorPWM(mL);
    setMotorPWM(mR);
    
    delay_ms(duration);

    (*mL).direction = 1;
    (*mR).direction = 1;

    setMotorPWM(mL);
    setMotorPWM(mR);
}

// Function which performs buggy action after reaching a card which is:
// - reversing either a small amount (to give room for the turn) or reversing one square depending on card
// - turning by a certain number of degrees
// - moving forwards once more

// Inputs:
// *mL: pointer to the left motor
// *mR: pointer to the right motor
// angleOfTurn: clockwise angle through which buggy turns
// reverseBuggyOneSquare: boolean which if true means buggy reverses one square, else it just reverses to give room for the turn

// output: none
void performBuggyActionAccordingToCard(DC_motor *mL, DC_motor *mR, unsigned int angleOfTurn, bool reverseBuggyOneSquare){
    if (reverseBuggyOneSquare){
        reverseBuggy(mL, mR, 25, 3250);
    }
    else{
        reverseBuggy(mL, mR, 15, 1000);
    }

    turnBuggy(mL, mR, angleOfTurn);
    fullSpeedAhead(mL, mR, motorPower);
}