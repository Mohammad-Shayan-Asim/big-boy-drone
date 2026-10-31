#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "uart.h"
#include "i2c.h"
#include "mpu6500.h"
#include "delay.h"
#include "pwm.h"
#include "stdio.h"

#define PORT_F 0x20

enum Lights {
  none = 0x00,
  red = 0x02,
  blue = 0x04,
  green = 0x08,
};

void PortF_Init() {
  SYSCTL_RCGCGPIO_R |= PORT_F;
  while(~SYSCTL_PRGPIO_R & PORT_F){};

  GPIO_PORTF_DEN_R |= 0xE;
  GPIO_PORTF_DIR_R |= 0xE;
};

void FP_Enable() {
  NVIC_CPAC_R |= 0x00F00000;
}

void FP_Disable() {
  NVIC_CPAC_R &= ~0x00F00000;
}

void ToggleLed(enum Lights Led) {
  GPIO_PORTF_DATA_R = Led;
}

int main(void){
  PortF_Init();
  UART0_Init();

  // if (I2C_Init() < 0) {
  //   UART0_Print("I2C initialization failed!\r\n");
  //   return -1;
  // }


  // if (MPU6500_Init() < 0) {
  //   UART0_Print("MPU6500 initialization failed!\r\n");
  //   return -1;
  // }


  if (PWM0_Init() < 0) {
    // UART0_Print("PWM0 initialization failed!\r\n");
    ToggleLed(red);
    return -1;
  }


  // UART0_Print("initialization successful!\r\n");
  ToggleLed(green);
  delay_ms(1000);
  ToggleLed(none);


  PWM0_WritePPMWidth(PPM_MIN);
  PWM0_StartOutput();

  ToggleLed(blue);
  delay_ms(5000);


  ToggleLed(green);



  for (uint16_t w = 1000; w <= 1500; w += 10) {
      PWM0_WritePPMWidth(w);              
      delay_ms(50);
  }

  delay_ms(5000);   
  
  for (uint16_t w = 1500; w > 1000; w -= 10) {
      PWM0_WritePPMWidth(w);              
      delay_ms(50);
  }

  // PWM0_WritePPMWidth(PPM_MIN);
  ToggleLed(none);


  while(1){}
}

