#include "delay.h"

void delay_ms(uint32_t ms) {
    // disable systick during setup
    NVIC_ST_CTRL_R = 0x00;
    
    // 16 MHz * 1 ms - 1
    NVIC_ST_RELOAD_R = 15999; 

    // set TMR_EN and CLK_SRC
    NVIC_ST_CTRL_R = 0x05; 

    // wait until count flag is set
    while(ms--){
        while((NVIC_ST_CTRL_R & (1<<16)) == 0){}
    }

    // stop systick
    NVIC_ST_CTRL_R = 0x00; 
}