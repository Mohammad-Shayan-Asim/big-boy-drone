
#include "mpu6500.h"
#include "delay.h"
#include "i2c.h"
#include "types.h"

static  sensor_offset_t sensor_offsets;


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
    // reset never completed
    if (reset & DEVICE_RESET) return -2;   

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
    accel_config |= MPU6500_ACCELEROMETER_RANGE_8G << 3;          

    // update to ACCEL_FS_SEL (accelerometer configuration) 
    if (I2C_RegisterWrite(MPU_ADDR_ADC0, MPU6500_REG_ACCEL_CONFIG, &accel_config, 1) < 0) return -1;

    return 0;
}


int MPU6500_Read(uint8_t* buffer) {
    // Performs a burst read of all sensor values (ACCEL, TEMP, GYRO)
    return I2C_RegisterRead(MPU_ADDR_ADC0, MPU_REG_ACCEL_XOUT_H, buffer, 14) < 0 ? -1 : 0;
}

void MPU6500_IntPinInit(void) {
    // Port E clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;               
    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4)) {}

    GPIO_PORTE_DIR_R   &= ~0x04;                 
    GPIO_PORTE_DEN_R   |=  0x04;

    // reads low if unplugged
    GPIO_PORTE_PDR_R   |=  0x04;                   
}

int MPU6500_DataReady(void) {
    return (GPIO_PORTE_DATA_R & 0x04) != 0;
}

// int MPU6500_SampleConfig(void) {
//     // SMPLRT_DIV (0x19) and CONFIG (0x1A) are consecutive
//     uint8_t cfg[2] = { 0x00, 0x03 };               // 1 kHz, gyro DLPF ~41 Hz
//     if (I2C_RegisterWrite(MPU_ADDR_ADC0, MPU6500_REG_SMPLRT_DIV, cfg, 2) < 0) return -1;

//     uint8_t acc2 = 0x03;                           // accel DLPF ~41 Hz, 1 kHz
//     if (I2C_RegisterWrite(MPU_ADDR_ADC0, MPU6500_REG_ACCEL_CONFIG2, &acc2, 1) < 0) return -1;
//     return 0;
// }

// int MPU6500_IntConfig(void) {
//     // INT_PIN_CFG (0x37) and INT_ENABLE (0x38) are consecutive
//     // 0x30 = LATCH_INT_EN | INT_ANYRD_2CLEAR (active high, push-pull)
//     // 0x01 = RAW_RDY_EN
//     uint8_t cfg[2] = { 0x30, 0x01 };
//     return I2C_RegisterWrite(MPU_ADDR_ADC0, MPU6500_REG_INT_PIN_CFG, cfg, 2);
// }



int MPU6500_Setup(void) {
    int rc;
    MPU6500_IntPinInit();
    if ((rc = MPU6500_Init())        < 0) return rc;
    if ((rc = MPU6500_GyroConfig())  < 0) return -10;
    if ((rc = MPU6500_AccelConfig()) < 0) return -11;
    if ((rc = MPU6500_SampleConfig())< 0) return -12;
    if ((rc = MPU6500_IntConfig())   < 0) return -13;
    delay_ms(100);                                         // let the sensor settle
    return 0;
}


uint16_t __bswap16 (uint16_t x) {
    return (x << 8) | (0xFF00 & x);
}

void MPU6500_Parse(const uint8_t *b, raw_sensor_t *r) {
    int16_t* values = (uint16_t*)b;
    r->ax =  __bswap16(values[0]);
    r->ay =  __bswap16(values[1]);
    r->az =  __bswap16(values[2]);
    r->gx =  __bswap16(values[3]);
    r->gy =  __bswap16(values[4]);
    r->gz =  __bswap16(values[5]);
}

int MPU6500_GetData(sensor_t *mag) {
    uint8_t buffer[14];
    raw_sensor_t raw;

    if (MPU6500_Read(buffer) < 0) return -1;
    MPU6500_Parse(buffer, &raw);

    mag->ax = (raw.ax - sensor_offsets.ax) / MPU6500_ACC_LSB_PER_G;
    mag->ay = (raw.ay - sensor_offsets.ay) / MPU6500_ACC_LSB_PER_G;
    mag->az = (raw.az - sensor_offsets.az) / MPU6500_ACC_LSB_PER_G;

    mag->gx = (raw.gx - sensor_offsets.gx) / MPU6500_GYRO_LSB_PER_DPS;
    mag->gy = (raw.gy - sensor_offsets.gy) / MPU6500_GYRO_LSB_PER_DPS;
    mag->gz = (raw.gz - sensor_offsets.gz) / MPU6500_GYRO_LSB_PER_DPS;
    return 0;
}

static int checkMinMax(int16_t* array, int size, int16_t diff_max) {
    int16_t min = array[0], max = array[0];
    
    for(int i=0; i<size; i++) {
        if(array[i] < min) min = array[i];  
        if(array[i] > max) max = array[i];  
    }
    return (max - min) < diff_max;
}


static int calibrateSensor(int16_t maxDifferenceGyro, int16_t maxDifferenceAcc) {
    static int16_t sensor_buffer[6][14];
    static const uint8_t buf_length = 14;
    uint8_t buf[14];

    for (uint8_t i = 0; i < buf_length; i++) {
        while (!dataReadyMPU6500()){}
        if (MPU6500_Read(buf) < 0) return -1;
        MPU6500_Parse(buf, sensor_buffer[i]);
        delay(10);
    }

    for (uint8_t axis = 0; axis < 3; axis++) {
        if (!checkMinMax(sensor_buffer[axis], buf_length, maxDifferenceAcc))
            return -1; // Return error

        if (!checkMinMax(sensor_buffer[3 + axis], buf_length, maxDifferenceGyro))
            return -1; // Return error
    }

    for (uint8_t i = 0; i < 6; i++) {
        for (uint8_t j = 1; j < buf_length; j++)
            sensor_buffer[i][0] += sensor_buffer[i][j]; 
        
        sensor_buffer[i][0] /= buf_length;
    }

    uint16_t* hack = (uint16_t*)&sensor_buffer;
    for(int i=0; i<6; i++){
        hack[i] = sensor_buffer[i][0];
    }
    return 0; // No error
}

