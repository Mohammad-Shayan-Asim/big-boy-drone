#include "pwm.h"
#include "delay.h"

void WritePPMWidth(int motor, uint16_t width) {
    if (width < PPM_MIN) width = PPM_MIN;
    if (width > PPM_MAX) width = PPM_MAX;

    volatile uint32_t* cmp = motor == 0? &PWM0_0_CMPA_R : motor == 1? &PWM0_0_CMPB_R: motor == 2? &PWM0_1_CMPA_R : &PWM0_1_CMPB_R;
    uint32_t  load = motor <= 1? PWM0_0_LOAD_R: PWM0_1_LOAD_R;

    *cmp = load * (2500 - width) / 2500;
}

int initPPM(void) {
  SYSCTL_RCGCPWM_R  |= SYSCTL_RCGCPWM_R0;
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

  while (~SYSCTL_PRPWM_R & SYSCTL_PRPWM_R0) {};
  while(~SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1){};

  GPIO_PORTB_DEN_R |= 0xF0;
  GPIO_PORTB_AFSEL_R |= 0xF0; 
  GPIO_PORTB_DR4R_R |= 0xF0;

  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0x0000FFFF) | 0x44440000;

  SYSCTL_RCC_R &= ~SYSCTL_RCC_USEPWMDIV; 

  PWM0_0_CTL_R = 0x0;
  PWM0_1_CTL_R = 0x0;

  PWM0_0_GENB_R = PWM0_1_GENB_R = PWM_0_GENB_ACTLOAD_ONE | PWM_0_GENB_ACTCMPBD_ZERO;
  PWM0_0_GENA_R = PWM0_1_GENA_R = PWM_0_GENA_ACTLOAD_ONE | PWM_0_GENA_ACTCMPAD_ZERO;

  PWM0_0_LOAD_R = PWM0_1_LOAD_R = 39999; 

  writePPMAllOff();

  PWM0_0_CTL_R |= PWM_0_CTL_ENABLE;
  PWM0_1_CTL_R |= PWM_1_CTL_ENABLE;

  PWM0_ENABLE_R |= 0x0F;
  return 1;
}

void writePPMAllOff(void) {
    for (int i = 0; i < 4; i++)
        WritePPMWidth(i, PPM_MIN);
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
    float v = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    if (v < out_min) v = out_min;
    if (v > out_max) v = out_max;
    return v;
}

void updateMotorsAll(float *values) {
    // float maxMotor = values[0];
    // for (int i = 1; i < 4; i++)
    //     if (values[i] > maxMotor) maxMotor = values[i];

    // if (maxMotor > MAX_MOTOR_OUT) {
    //     for (int i = 0; i < 4; i++)
    //         values[i] -= maxMotor - MAX_MOTOR_OUT;
    // }

    for (int i = 0; i < 4; i++)
        WritePPMWidth(i, (uint16_t)mapf(values[i], MIN_MOTOR_OUT, MAX_MOTOR_OUT, PPM_MIN, PPM_MAX));
}