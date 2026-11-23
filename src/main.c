// #include "stdint.h"
#include "tm4c123gh6pm.h"
#include "uart.h"
#include "i2c.h"
#include "mpu6500.h"
#include "delay.h"
#include "pwm.h"
#include "stdio.h"
#include "mav.h"
#include "filter.h"

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

#define TIMER_DELAY 0.002
#define CLOCK_SPEED 16000000

volatile angle_t angle;
volatile sensor_t imu;
volatile alpha_t alpha;

volatile uint32_t time_since_boot = 0; 



void Timer0A_Init() {
  SYSCTL_RCGCTIMER_R |= 0x01;                 
  while (!(SYSCTL_PRTIMER_R & 0x01)) {}
  TIMER0_CTL_R  = 0;                        
  TIMER0_CFG_R  = 0;                         
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;   
  TIMER0_TAILR_R = CLOCK_SPEED * TIMER_DELAY - 1;        
  TIMER0_ICR_R  = TIMER_ICR_TATOCINT;
  
  TIMER0_IMR_R  = TIMER_IMR_TATOIM;           
  NVIC_EN0_R    = 1 << 19;            
  NVIC_PRI4_R = (NVIC_PRI4_R & 0x1FFFFFFF) | (2u << 29);     
  TIMER0_CTL_R  |= TIMER_CTL_TAEN;
}




void Timer0A_ISR(void) {

    if (MPU6500_GetData(&imu) == 0){
      updateAngles(&imu, &angle, &alpha, TIMER_DELAY);
    }
    time_since_boot += 2;

    if((time_since_boot % 10) == 0) {
      mavlink_attitude_send(&angle, &imu);    
    }
    
    if((time_since_boot % 1000) == 0){
      GPIO_PORTF_DATA_R ^= 0x04;
      mavlink_heartbeat_append();
    }

    TIMER0_ICR_R = TIMER_ICR_TATOCINT;
}

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
  // UART1_Init();

  FP_Enable();


  UART0_Print("Tiva C Launchpad says hello!\r\n");

  // if (I2C_Init() < 0) {
  //   UART0_Print("I2C initialization failed!\r\n");
  //   return -1;
  // }


  // if (MPU6500_Setup() < 0) {
  //   UART0_Print("MPU6500 initialization failed!\r\n");
  //   return -1;
  // }

  // alpha.X = 0.02f;
  // alpha.Y = 0.02f;
  // alpha.Z = 0.02f;
  // ToggleLed(red);

  // Timer0A_Init();


  // int ticks = 0;
  // for(;;){
  //   delay_ms(10);
  //   if(++ticks>=100){
  //     ticks=0;
  //     mavlink_heartbeat_append();
  //   }

  //   mavlink_pop_queue();
  //   // UART0_Print("heartbeat sent\r\n");
  // }



 

  // FP_Enable();

  if (initPPM() < 0) {
    ToggleLed(red);
    return -1;
  }
  ToggleLed(blue);

  float values[] = {50.f, 50.0f, 50.0f, 50.0f};   
  writePPMAllOff();
  delay_ms(5000);

  updateMotorsAll(values);
  delay_ms(10000);
  writePPMAllOff();







  // float values[] = {100.0f, 100.0f, 100.0f, 100.0f};   
  // updateMotorsAll(values);




  // // writePPMAllOff();

  // delay_ms(3000);      
  // GPIO_PORTF_DEN_R |=  0x10;    
  // GPIO_PORTF_DIR_R &= ~0x10;                 
  // GPIO_PORTF_PUR_R |=  0x10;               
  // GPIO_PORTF_DATA_R |= 0x10;      

 
  // while ((GPIO_PORTF_DATA_R & 0x10)) { }

  // ToggleLed(red);
  // values[0] = 0.0f;
  // values[1] = 0.0f;
  // values[2] = 0.0f;
  // values[3] = 0.0f;
  // updateMotorsAll(values);
  

  // delay_ms(2000);
  // ToggleLed(blue);
  // writePPMAllOff();

  while(1) { 
      // mavlink_pop_queue();
  }



  // GPIO_PORTF_DEN_R |= 1<<4;
  // GPIO_PORTF_DIR_R &= ~(1<<4);
  // GPIO_PORTF_PUR_R |= 1<<4;
  // GPIO_PORTF_DATA_R = 1<<4;

 





  if (initPPM() < 0) {
    // UART0_Print("PWM0 initialization failed!\r\n");
    ToggleLed(red);
    return -1;
  }


}

