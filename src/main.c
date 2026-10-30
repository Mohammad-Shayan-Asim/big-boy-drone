#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "uart.h"
#include "i2c.h"
#include "stdio.h"
#include "mpu6500.h"
#include "delay.h"
#include "stdio.h"

#define PORT_F 0x20

unsigned long Led;

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

int main(void){
  PortF_Init();
  UART0_Init();

  if (I2C_Init() < 0) {
    UART0_Print("I2C initialization failed!\r\n");
    return -1;
  }


  if (MPU6500_Init() < 0) {
    UART0_Print("MPU6500 initialization failed!\r\n");
    return -1;
  }


  // if (PWM0_Init() < 0) {
  //   UART0_Print("PWM0 initialization failed!\r\n");
  //   return -1;
  // }

  UART0_Print("initialization successful!\r\n");


  while(1){
    uint16_t d[7];
    if (MPU6500_Read((uint8_t*)d) < 0) {
      UART0_Print("MPU6500 read failed!\r\n");
      return -1;
    }
    

    for(int i=0; i<7 ; i++) {
      d[i] = (d[i] << 8) | (d[i] >> 8);
    }

    // UART0_Print("what");

    int16_t* cast = (int16_t*)d;

    char msg[128];
    sprintf(msg,
            "%i, %i, %i, %i, %i, %i, %i\r\n",
            cast[0], cast[1], cast[2],
            cast[3], cast[4], cast[5], 
            cast[6]);
   

    UART0_Print(msg);




    delay_ms(20);



  }
  // while(1) {
  //   GPIO_PORTF_DATA_R = 0x02;
  //   delay_ms(500);
  //   GPIO_PORTF_DATA_R = 0x04;
  //   delay_ms(500);
  //   GPIO_PORTF_DATA_R = 0x08;
  //   delay_ms(500);





  // }

}

