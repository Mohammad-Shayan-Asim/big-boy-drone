#ifndef __PWM_H__
#define __PWM_H__
#include "stdint.h"
#include "tm4c123gh6pm.h"

// Internal motor values are in the range [0:100]
#define MIN_MOTOR_OUT (0.0f)
#define MAX_MOTOR_OUT (100.0f)

#define PPM_MIN 1000 // From SimonK firmware, this must be calibrated for our use case
#define PPM_MAX 2000 // From SimonK firmware. this must be calibrated for our use case

int initPPM(void);
void writePPMAllOff(void);
void updateMotorsAll(float *values);

float mapf(float x, float in_min, float in_max, float out_min, float out_max);


#endif