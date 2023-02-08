#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>
#include <stdbool.h>

#define _XTAL_FREQ 64000000

// Global variables

unsigned int forwardSequenceDistanceCounter;
unsigned int *pointerforwardSequenceDistanceCounter;

unsigned int reverseSequenceDistanceCounter;
unsigned int *pointerreverseSequenceDistanceCounter;

unsigned int turnDuration = 125;
unsigned int *pointerturnDuration;

unsigned int motorPower = 30;

typedef struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    char brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
} DC_motor;

//function prototypes
DC_motor init_DC_motor(bool isLeftMotorBool);
void initDCmotorsPWM();
void setMotorPWM(DC_motor *m);
void stopBuggy(DC_motor *mL, DC_motor *mR);
void turnBuggy(DC_motor *mL, DC_motor *mR, unsigned int clockwiseTurnAngleInDegrees);
void fullSpeedAhead(DC_motor *mL, DC_motor *mR, char power);
void reverseBuggy(DC_motor *mL, DC_motor *mR, unsigned int power, unsigned int duration);
void performBuggyActionAccordingToCard(DC_motor *mL, DC_motor *mR, unsigned int angleOfTurn, bool reverseBuggyOneSquare);

#endif
