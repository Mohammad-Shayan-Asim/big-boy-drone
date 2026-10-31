#ifndef __PWM_H__
#define __PWM_H__
#include "stdint.h"
#include "tm4c123gh6pm.h"

#define PPM_MIN 1000 // From SimonK firmware, this must be calibrated for our use case
#define PPM_MAX 2000 // From SimonK firmware. this must be calibrated for our use case

int PWM0_Init();
void PWM0_WritePPMWidth(uint16_t width);
void PWM0_StartOutput();

#endif