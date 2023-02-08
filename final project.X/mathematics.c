// mathematics.c
// mathematical functions

#include <xc.h>
#include <stdbool.h>

// Function which returns the absolute percentage change of the new_value with respect to the reference value

// input:
// - reference_value: original value
// - new_value: changed value

// output: absolute percentage change
float absolutePercentageChange(float reference_value, float new_value){

    float tmp;

    if(reference_value>new_value){
        tmp = ((reference_value-new_value)/reference_value) * 100;
    }

    if (new_value>reference_value){
        tmp = ((new_value-reference_value)/reference_value) * 100;
    }
    
    return tmp;
}