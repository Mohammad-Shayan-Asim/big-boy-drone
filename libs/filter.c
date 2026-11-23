#include "filter.h"

// https://mwrona.com/posts/accel-roll-pitch/
// complementary filter
void updateAngles(sensor_t *mag, angle_t *angle, alpha_t *alpha, float dt) {
    float accelRoll, accelPitch, gyroRoll, gyroPitch, gyroYaw;

    float g = sqrtf(mag->ax*mag->ax + mag->ay*mag->ay +  mag->az*mag->az);

    gyroRoll = angle->X + dt*mag->gx;
    gyroPitch = angle->Y + dt*mag->gy;
    gyroYaw = angle->Z + dt*mag->gz;
    
    if( g < 0.85f || g > 1.15f ) {
        angle->X = gyroRoll;
        angle->Y = gyroPitch;
        angle->Z = gyroYaw;
        return; // do not use accelerometer if total acceleration is not in the correct range
    }

    accelPitch = asin(mag->ax / g);
    accelRoll = atan2f(mag->ay, mag->az);
 
    angle->X = accelRoll*alpha->X + (1-alpha->X)*gyroRoll;
    angle->Y = accelPitch*alpha->Y + (1-alpha->Y)*gyroPitch;
    angle->Z = gyroYaw;
}



