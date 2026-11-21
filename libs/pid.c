#include "pid.h"

void  PIDController_Init(PIDController *pid) {
    pid->integrator = 0.0f;
    pid->prevError = 0.0f;
    pid->differentiator = 0.0f;
    pid->prevMeasurement = 0.0f;
    pid->out = 0.0f;
};

float PIDController_Update(PIDController *pid, float setpoint, float measurement) {

    float error = setpoint - measurement;
    
    pid->integrator += pid->Ki * pid->T * error;
    
    if(pid->integrator < pid->limMinInt) {
        pid->integrator = pid->limMinInt;
    }
    if(pid->integrator > pid->limMaxInt) {
        pid->integrator = pid->limMaxInt;
    }

    pid->differentiator = pid->Kd * (pid->prevMeasurement - measurement) / pid->T;


    pid->out = pid->differentiator + pid->integrator + pid->Kp * error;
    if(pid->out < pid->limMin) {
        pid->out = pid->limMin;
    }
    if(pid->out > pid->limMax) {
        pid->out = pid->limMax;
    }

    pid->prevError = error;
    pid->prevMeasurement = measurement;
    return pid->out;
};

