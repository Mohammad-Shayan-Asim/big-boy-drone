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

  FP_Enable();

  if (initPPM() < 0) {
    ToggleLed(red);
    return -1;
  }
  ToggleLed(blue);

  // float values[] = {30.0f, 0.0f, 0.0f, 0.0f};   
  // writePPMAllOff();
  // delay_ms(3000);

  // updateMotorsAll(values);



  // float values[] = {100.0f, 0.0f, 0.0f, 0.0f};   
  // updateMotorsAll(values);


  // writePPMAllOff();

  // delay_ms(3000);      
  // GPIO_PORTF_DEN_R |=  0x10;    
  // GPIO_PORTF_DIR_R &= ~0x10;                 
  // GPIO_PORTF_PUR_R |=  0x10;               
  // GPIO_PORTF_DATA_R |= 0x10;      
  // updateMotorsAll(values);

 
  // while ((GPIO_PORTF_DATA_R & 0x10)) { }

  // ToggleLed(red);
  // values[0] = 0.0f;
  // updateMotorsAll(values);
  

  // delay_ms(2000);
  // ToggleLed(blue);
  // writePPMAllOff();

  while(1) {

  }



  // GPIO_PORTF_DEN_R |= 1<<4;
  // GPIO_PORTF_DIR_R &= ~(1<<4);
  // GPIO_PORTF_PUR_R |= 1<<4;
  // GPIO_PORTF_DATA_R = 1<<4;

 


  // if (I2C_Init() < 0) {
  //   UART0_Print("I2C initialization failed!\r\n");
  //   return -1;
  // }


  // if (MPU6500_Init() < 0) {
  //   UART0_Print("MPU6500 initialization failed!\r\n");
  //   return -1;
  // }


  if (initPPM() < 0) {
    // UART0_Print("PWM0 initialization failed!\r\n");
    ToggleLed(red);
    return -1;
  }


}

