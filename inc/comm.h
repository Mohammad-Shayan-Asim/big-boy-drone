#ifndef __COMM_H__
#define __COMM_H__
#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "uart.h"
#include "mavlink.h"
#include "startup.h"



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


int Mavlink_Init() {
    _register_interrupt(NVIC_UART1, Mavlink_Parse);
    return 0;
}

void Mavlink_Parse() {
    uint8_t magic;
    UART1_Read(&magic, 1);
    if (magic != MAVLINK_STX_MAVLINK1) {
        return;
    }

    uint8_t len;
    UART1_Read(&len, 1);
    
    uint8_t packet[512];
    memset(packet, NULL, 512);
    UART1_Read(packet, len); 

    uint32_t msg_id = packet[7] | packet[8] | packet[9];

    switch (msg_id) {
        case MAVLINK_MSG_ID_HEARTBEAT:
            UART0_Print("heartbeat received!\n");
    }
}

#endif