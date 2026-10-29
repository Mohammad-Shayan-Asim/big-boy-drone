#include "uart.h"


void UART0_Init(void) {
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
  while (~SYSCTL_PRUART_R & SYSCTL_PRUART_R0) {}
  while (~SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0) {}

  // disable while configuring
  UART0_CTL_R = 0;                 
  
  // 115200 baud at 16 MHz
  UART0_IBRD_R = 8;                        
  UART0_FBRD_R = 44;

  // 8 data bits, no parity, 1 stop
  UART0_LCRH_R = UART_LCRH_WLEN_8;          
  UART0_CTL_R = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;

  // PA0 (RX), PA1 (TX)
  GPIO_PORTA_AFSEL_R |= 0x03;               
  GPIO_PORTA_DEN_R   |= 0x03;

  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011;
}

void UART0_PutChar(char c) {
  while (UART0_FR_R & UART_FR_TXFF) {}      // wait while transmit FIFO is full
  UART0_DR_R = c;
}

void UART0_Print(const char *s) {
  while (*s) UART0_PutChar(*s++);
}
