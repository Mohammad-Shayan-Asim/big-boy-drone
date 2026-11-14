#ifndef __UART_H__
#define __UART_H__
#include "stdint.h"
#include "tm4c123gh6pm.h"

#define NVIC_UART1 6

void UART0_Init(void);
void UART0_PutByte(char c);
void UART0_Print(const char *s);

void UART1_Init(void);
void UART1_Read(uint8_t* bytes, int n);
void UART1_RX_Clear();

#endif