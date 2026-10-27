
//main.c
//Toggles the Red LED of TM4C Launchpad when SW1(PF4) is pushed and held
//PF4 is negative logic,i.e 0 is on and non-zero is off
#include "stdint.h"
#include "tm4c123gh6pm.h"

#define PORT_F 0x20

unsigned long Led;

void PortF_Init() {

  SYSCTL_RCGCGPIO_R |= PORT_F;
  while(~SYSCTL_PRGPIO_R & PORT_F){};

  GPIO_PORTF_DEN_R |= 0xE;
  GPIO_PORTF_DIR_R |= 0xE;
};

void Delay(void){unsigned long volatile time;
  time = 800000;
  while(time){
		time--;
  }
}


int main(void){
  PortF_Init();

  while(1){
   //if(PF4 == 0x0){
    Led = 0x02;            // reverse value of LED
    GPIO_PORTF_DATA_R = Led;    // write value to PORTF DATA register,toggle led
    Delay();
    Led = 0x04;            // reverse value of LED
    GPIO_PORTF_DATA_R = Led;   // write value to PORTF DATA register,toggle led
    Delay();
    Led = 0x08;            // reverse value of LED
    GPIO_PORTF_DATA_R = Led;   // write value to PORTF DATA register,toggle led
    Delay();
    /*Led = 0x0A;            // reverse value of LED
    GPIO_PORTF_DATA_R = Led;   // write value to PORTF DATA register,toggle led
    Delay();*/
   // }
   break;
  }
    GPIO_PORTF_DATA_R = 0x0;   // write value to PORTF DATA register,toggle led

}

// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06
