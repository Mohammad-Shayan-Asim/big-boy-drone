#include "mav.h"
#include "types.h"

extern  uint32_t time_since_boot;

static const uint32_t system_id = 1;
static const uint8_t component_id = 1;

static volatile int msg_queue_tail = 0;
static volatile int msg_queue_head = 0;

static mavlink_message_t msg_queue[QUEUE_SIZE];

void mavlink_queue_append(const mavlink_message_t* msg) {
    NVIC_DIS0_R = 1 << NVIC_UART1; 
    if((msg_queue_tail + 1)%QUEUE_SIZE == msg_queue_head) {
        NVIC_EN0_R = 1 << NVIC_UART1;  return;
    }
    msg_queue[msg_queue_tail] = *msg;
    msg_queue_tail = (msg_queue_tail + 1)%QUEUE_SIZE;
    NVIC_EN0_R = 1 << NVIC_UART1; 
}

void mavlink_pop_queue() {
    if(msg_queue_head == msg_queue_tail) return;
    mavlink_send_message(&msg_queue[msg_queue_head]);
    msg_queue_head = (msg_queue_head + 1)%QUEUE_SIZE;
}

void mavlink_send_message(const mavlink_message_t* msg) {
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t len = mavlink_msg_to_send_buffer(buffer, msg);
    for(int i=0; i<len; i++) {
        UART1_PutByte(buffer[i]);
    }
}

void mavlink_parse() {
    for(;;){      
        uint8_t byte;
        if(UART1_ReadByte(&byte) < 0) {
            break;
        }

        static mavlink_message_t msg;
        static mavlink_status_t status;

        if (mavlink_parse_char(MAVLINK_COMM_0, byte, &msg, &status)) {
            switch (msg.msgid) {
            case MAVLINK_MSG_ID_PARAM_REQUEST_LIST: {
                mavlink_message_t response_msg;
                mavlink_msg_param_value_pack(system_id, component_id, &response_msg, "param", 0.0, MAV_PARAM_TYPE_REAL32, 1, 0);
                mavlink_queue_append(&response_msg);
                break;
            }

            case MAVLINK_MSG_ID_COMMAND_LONG: {
                mavlink_command_long_t cmd;
                mavlink_msg_command_long_decode(&msg, &cmd);
                if(cmd.target_system != system_id || (cmd.target_component != component_id && cmd.target_component != 0)) break;

                mavlink_message_t command_ack;
                mavlink_msg_command_ack_pack(system_id, component_id, &command_ack, cmd.command, MAV_RESULT_UNSUPPORTED, 0, 0, msg.sysid, msg.compid);
                mavlink_queue_append(&command_ack);
                break;
            }

            case MAVLINK_MSG_ID_MISSION_REQUEST_LIST: {
                mavlink_mission_request_list_t req;
                mavlink_msg_mission_request_list_decode(&msg, &req);
                if(req.target_system != system_id || (req.target_component != component_id && req.target_component != 0)) break;

                mavlink_message_t mission_count;
                mavlink_msg_mission_count_pack(system_id, component_id, &mission_count, msg.sysid, msg.compid, 0, req.mission_type, 0);
                mavlink_queue_append(&mission_count);
                break;
            }
        
            default:
                break;
            }
        }
    }
    UART1_RX_Clear();
}

void UART1_ISR() {
    mavlink_parse();
}

void mavlink_heartbeat_append() {
    mavlink_message_t msg;
    mavlink_msg_heartbeat_pack(system_id, component_id, &msg, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_GENERIC, MAV_MODE_AUTO_DISARMED, 0, MAV_STATE_ACTIVE);
    mavlink_queue_append(&msg);
}

void mavlink_attitude_send(angle_t* angle, sensor_t* ag) {
    mavlink_message_t msg;
    mavlink_msg_attitude_pack(system_id, component_id, &msg, time_since_boot, angle->X, angle->Y, angle->Z, ag->gx, ag->gy, ag->gz);
    mavlink_queue_append(&msg);
}
