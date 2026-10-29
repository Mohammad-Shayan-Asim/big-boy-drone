
#ifndef __I2C_H__
#define __I2C_H__
#include "stdint.h"
#include "tm4c123gh6pm.h"

int I2C_Init();
int I2C_RegisterWrite(uint8_t addr, uint8_t reg, uint8_t* buff, int size);
int I2C_RegisterRead(uint8_t addr, uint8_t reg, uint8_t* buff, int size);

#endif