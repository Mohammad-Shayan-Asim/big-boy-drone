#ifndef __COMM_H__
#define __COMM_H__
#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "uart.h"
#include "mavlink.h"
#include "types.h"
// #include "startup.h"



// MANUAL_CONTROL (69)
// https://mavlink.io/en/services/manual_control.html
// normalized to the range [-1000,1000]


// COMMAND_LONG (76)
// https://mavlink.io/en/services/command.html


// SCALED_IMU (26)
// https://mavlink.io/en/messages/common.html#SCALED_IMU


// SERVO_OUTPUT_RAW (36)
// https://mavlink.io/en/messages/common.html#SERVO_OUTPUT_RAW


// DEBUG_VECT (250)
// https://mavlink.io/en/messages/common.html#DEBUG_VECT


// ATTITUDE (30)
// https://mavlink.io/en/messages/common.html#ATTITUDE


// PARAM_REQUEST_LIST (21)
// https://mavlink.io/en/services/parameter.html

#define QUEUE_SIZE 32


void mavlink_queue_append(const mavlink_message_t* msg);
void mavlink_pop_queue();

void mavlink_send_message(const mavlink_message_t* msg);
void mavlink_parse();
// void UART1_ISR();
void mavlink_heartbeat_append();
void mavlink_attitude_send(angle_t* angle, sensor_t* ag);

#endif