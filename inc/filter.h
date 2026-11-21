#ifndef __FILTER_H__
#define __FILTER_H__
#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "types.h"
#include "math.h"


void updateAngles(sensor_t *mag, angle_t *angle, alpha_t *alpha, float dt);

// void rotateV(sensor_t *v, const angle_t *angle);
#endif