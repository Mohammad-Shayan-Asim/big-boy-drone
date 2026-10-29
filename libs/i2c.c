#include "i2c.h"

#define I2C_READ  1
#define I2C_WRITE 0

// waits until bus is free
void I2C_BusBusy(){
    while(I2C0_MCS_R & I2C_MCS_BUSBSY){}
}

// waits until the frame is transmitted/received
void I2C_Busy(){
    while(I2C0_MCS_R & I2C_MCS_BUSY){}
}

// stops I2C0 transmission, if an error occurs
int I2C_Wait(){
    I2C_Busy();
    int err = I2C0_MCS_R & I2C_MCS_ERROR;
    if(err)
        I2C0_MCS_R = I2C_MCS_STOP;
    return err;
}
 
int I2C_Init() {
  // enable clock to I2C0 and Port B
  SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

  // Poll until writes have taken place
  while (~SYSCTL_PRI2C_R & SYSCTL_PRI2C_R0) {}
  while(~SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R1){};

  // enable PB2 and PB3
  GPIO_PORTB_DEN_R |= 0xC; 

  // enable alternate functionality for PB2 and PB3
  GPIO_PORTB_AFSEL_R |= 0xC; 

  // set alternate functionality as I20SCL and I2C0SDA
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF00FF) | 0x00003300; 
  
  // configure PB3 as open-drain
  GPIO_PORTB_ODR_R |= 0x08; 

  // represents the number of system clock periods in one SCL clock period
  I2C0_MTPR_R = 7; 
  
  // enable master mode, disable slave mode
  I2C0_MCR_R = 0x10; 

  return 0;
}

int I2C_RegisterWrite(uint8_t addr, uint8_t reg, uint8_t* buff, int size) {
    // do not write, if there is nothing to write
    if(size<=0) return -1;
    
    // wait until bus is not busy
    I2C_BusBusy(); 

    // specify slave address, and receive/transmit
    I2C0_MSA_R = addr<<1 | I2C_WRITE; 

    // specify the register we want to write to
    I2C0_MDR_R = reg; 

    // specify the first command     
    I2C0_MCS_R =  I2C_MCS_RUN| I2C_MCS_START;

    if (I2C_Wait()) return -1; 
  
    for(int i=0; i < size-1; i++){
        I2C0_MDR_R = buff[i];
        I2C0_MCS_R = I2C_MCS_RUN;
        if (I2C_Wait()) return -1;
    }
   
    I2C0_MDR_R = buff[size-1];
    I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_STOP;
    if (I2C_Wait()) return -1;
    return 0;
}

int I2C_RegisterRead(uint8_t addr, uint8_t reg, uint8_t* buff, int size) {
    // do not read, if buffer does not have capacity
    if (size <= 0) return -1; 

    // wait until bus is not busy
    I2C_BusBusy(); 
    
    // specify slave address, and receive/transmit
    I2C0_MSA_R = addr<<1 | I2C_WRITE;

    // specify the register we want to write to
    I2C0_MDR_R = reg;

    // specify the first command  
    I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_START;

    if (I2C_Wait()) return -1;

    I2C0_MSA_R = addr<<1 | I2C_READ;

    if(size == 1){
        I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_START | I2C_MCS_STOP;
        if (I2C_Wait()) return -1;
        buff[0] = I2C0_MDR_R;   
        return 0;
    }
    
    I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_START | I2C_MCS_ACK;
    if (I2C_Wait()) return -1;
    buff[0] = I2C0_MDR_R; 

    for (int i = 1; i < size-1; i++) {
        I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_ACK;
        if (I2C_Wait()) return -1;
        buff[i] = I2C0_MDR_R;                        
    }

    I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_STOP;
    if (I2C_Wait()) return -1;
    buff[size-1] = I2C0_MDR_R;
    return 0;   
}

// for more information refer to the tiva c TM4C123GH6PM datasheet. especially the I2C section.
