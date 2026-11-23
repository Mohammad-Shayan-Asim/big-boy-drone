#ifndef __MPU6500_H__
#define __MPU6500_H__
#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "types.h"


#define MPU_ADDR_ADC0 0x68
#define MPU_REG_PWR_MGMT_1 0x6B
#define DEVICE_RESET (1<<7)

#define MPU_REG_ACCEL_XOUT_H 59
#define MPU_REG_ACCEL_XOUT_L 60
#define MPU_REG_ACCEL_YOUT_H 61
#define MPU_REG_ACCEL_YOUT_L 62
#define MPU_REG_ACCEL_ZOUT_H 63
#define MPU_REG_ACCEL_ZOUT_L 64

#define MPU_REG_TEMP_XOUT_H 67
#define MPU_REG_TEMP_XOUT_L 67

#define MPU_REG_GYRO_XOUT_H 67
#define MPU_REG_GYRO_XOUT_L 60
#define MPU_REG_GYRO_YOUT_H 61
#define MPU_REG_GYRO_YOUT_L 62
#define MPU_REG_GYRO_ZOUT_H 63
#define MPU_REG_GYRO_ZOUT_L 64

#define MPU_REG_WHO_AM_I 117

#define MPU6500_REG_GYRO_CONFIG 27
#define MPU6500_REG_ACCEL_CONFIG 28
#define MPU6500_REG_ACCEL_CONFIG2   0x1D

#define  MPU6500_GYROSCOPE_RANGE_250DPS   0x00       
#define  MPU6500_GYROSCOPE_RANGE_500DPS   0x01       
#define  MPU6500_GYROSCOPE_RANGE_1000DPS  0x02        
#define  MPU6500_GYROSCOPE_RANGE_2000DPS  0x03     

#define MPU6500_ACCELEROMETER_RANGE_4G   0x01        
#define MPU6500_ACCELEROMETER_RANGE_2G   0x00         
#define MPU6500_ACCELEROMETER_RANGE_8G   0x02        
#define MPU6500_ACCELEROMETER_RANGE_16G  0x03        
#define MPU6500_REG_INT_PIN_CFG     0x37
#define MPU6500_REG_INT_ENABLE      0x38
#define MPU6500_REG_SMPLRT_DIV      0x19
#define MPU6500_REG_CONFIG          0x1A
#define MPU6500_GYRO_LSB_PER_DPS    16.4f
#define MPU6500_ACC_LSB_PER_G       4096.0f


int  MPU6500_Init(void);            // reset, wake, check WHO_AM_I
int  MPU6500_GyroConfig(void);      // +-2000 dps
int  MPU6500_AccelConfig(void);     // +-8 g
int  MPU6500_SampleConfig(void);    // 1 kHz, DLPF ~41 Hz
int  MPU6500_IntConfig(void);       // INT pin: active high, latched, data ready
void MPU6500_IntPinInit(void);      // PE2 as input
int  MPU6500_Setup(void);           // runs everything above in order
int  MPU6500_DataReady(void);       // 1 when INT (PE2) is high
int  MPU6500_Read(uint8_t *buffer); // 14 raw bytes
void MPU6500_Parse(uint8_t *b, raw_sensor_t *r);
int  calibrateSensor(int16_t maxDifferenceGyro, int16_t maxDifferenceAcc);
int  MPU6500_GetData(sensor_t *d); // read + offset + scale

#endif