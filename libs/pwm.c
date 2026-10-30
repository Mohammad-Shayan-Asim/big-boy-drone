#include "pwm.h"
#include "delay.h"

//width is in microseconds
void PWM0_WritePPMWidth(uint16_t width) {
    PWM0_0_CMPA_R = PWM0_0_LOAD_R * (2500 - width) / 2500;
}

int PWM0_Init() {
  // enable clock to PWM0 and Port B
  SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0;
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

  // poll until writes have taken place
  while (~SYSCTL_PRPWM_R & SYSCTL_PRPWM_R0) {}
  while(~SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1){};

  // enable PB6
  GPIO_PORTB_DEN_R |= 1<<6; 

  // enable alternate functionality for PB6 
  GPIO_PORTB_AFSEL_R |= 1<<6; 

  // set alternate functionality as M0PWM0 
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xF0FFFFFF) | 0x04000000;

  // use PWMDIV with RCC_PWMDIV_8
//   SYSCTL_RCC_R = (~SYSCTL_RCC_PWMDIV_M & SYSCTL_RCC_R) | SYSCTL_RCC_PWMDIV_8 | SYSCTL_RCC_USEPWMDIV;
  SYSCTL_RCC_R &= ~SYSCTL_RCC_USEPWMDIV; 

  // disable PWM0 generator 0 and set countdown mode
  PWM0_0_CTL_R = 0x0;

  // configure PWM_0_GENA
  PWM0_0_GENA_R = PWM_0_GENA_ACTZERO_NONE | PWM_0_GENA_ACTLOAD_ONE | PWM_0_GENA_ACTCMPAD_ZERO | PWM_0_GENA_ACTCMPAU_NONE;

  //16 Mhz / 400 Hz - 1, set period to 2.5 ms
  PWM0_0_LOAD_R = 39999;

  PWM0_WritePPMWidth(PPM_MIN);

  // start the timers in PWM0 generator 0.
  PWM0_0_CTL_R |= PWM_0_CTL_ENABLE;

  // disable pwm outputs for M0PWM0
  PWM0_ENABLE_R &= 0x01;
  return 0;
}

void PWM0_StartOutput(void) {
    PWM0_ENABLE_R |= 0x01;
}
