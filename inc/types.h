#ifndef __TYPES_H__
#define __TYPES_H__

typedef struct {
    float ax, ay, az;
    float gx, gy, gz;
    // float te;
} sensor_t;

typedef struct {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    // int16_t te;
}sensor_offset_t, raw_sensor_t;;

typedef struct {
    float X, Y, Z;
} angle_t;

typedef struct {
    float X, Y, Z;
} alpha_t;

#endif
