#ifndef __UART_H__
#define __UART_H__
#include "stdint.h"
#include "tm4c123gh6pm.h"

void UART0_Init(void);
void UART0_PutChar(char c);
void UART0_Print(const char *s);

#endif