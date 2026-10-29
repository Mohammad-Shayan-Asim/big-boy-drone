#include "mpu6500.h"
#include "delay.h"
#include "i2c.h"



int MPU6500_Init() {
    // DEVICE_RESET code, resets the internal registers and restores the default settings
    uint8_t reset = DEVICE_RESET; 
    uint8_t who_am_i;

    // set the DEVICE_RESET bit in the PWR_MGMT_1 
    if (I2C_RegisterWrite(MPU_ADDR_ADC0, MPU_REG_PWR_MGMT_1, &reset, 1) < 0) return -1;

    // set the timeout 1000 ms 
    int timeout = 10; 

    while(timeout--) {
        delay_ms(100);
        
        // read the DEVICE_RESET bit in the PWR_MGMT_1, which is cleared after reset 
        if (I2C_RegisterRead(MPU_ADDR_ADC0, MPU_REG_PWR_MGMT_1, &reset, 1) < 0) return -1;

        // check if the DEVICE_RESET bit is cleared
        if (~reset & DEVICE_RESET) break;
    }

    // read the WHO_AM_I register 
    if (I2C_RegisterRead(MPU_ADDR_ADC0, MPU_REG_WHO_AM_I, &who_am_i, 1)) return -1;

    // check if the WHO_AM_I value is valid
    return who_am_i == 0x70? 0 : -1;   
}

int MPU6500_GyroConfig() {
    uint8_t gyro_config;
    
    // read to GYRO_FS_SEL (gyroscope configuration) 
    if (I2C_RegisterRead(MPU_ADDR_ADC0, MPU6500_REG_GYRO_CONFIG, &gyro_config, 1) < 0) return -1;

    // clear gyroscope configuration
    gyro_config &= ~(3 << 0x03);  
    
    // set gyroscope configuration
    gyro_config |= MPU6500_GYROSCOPE_RANGE_2000DPS << 3;          

    // update to GYRO_FS_SEL (gyroscope configuration) 
    if (I2C_RegisterWrite(MPU_ADDR_ADC0, MPU6500_REG_GYRO_CONFIG, &gyro_config, 1) < 0) return -1;

    return 0;
}

int MPU6500_AccelConfig() {
    uint8_t accel_config;
    
    // read to ACCEL_FS_SEL (accelerometer configuration) 
    if (I2C_RegisterRead(MPU_ADDR_ADC0, MPU6500_REG_ACCEL_CONFIG, &accel_config, 1) < 0) return -1;

    // clear accelerometer configuration
    accel_config &= ~(3 << 0x03);  
    
    // set accelerometer configuration
    accel_config |= MPU6500_ACCELEROMETER_RANGE_16G << 3;          

    // update to ACCEL_FS_SEL (accelerometer configuration) 
    if (I2C_RegisterWrite(MPU_ADDR_ADC0, MPU6500_REG_ACCEL_CONFIG, &accel_config, 1) < 0) return -1;

    return 0;
}


int MPU6500_Read(uint8_t* buffer) {
    // Performs a burst read of all sensor values (ACCEL, TEMP, GYRO)
    return I2C_RegisterRead(MPU_ADDR_ADC0, MPU_REG_ACCEL_XOUT_H, buffer, 14) < 0 ? -1 : 0;
}


