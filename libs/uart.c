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
  UART0_LCRH_R = UART_LCRH_WLEN_8  | UART_LCRH_FEN;          
  UART0_CTL_R = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;

  // PA0 (RX), PA1 (TX)
  GPIO_PORTA_AFSEL_R |= 0x03;               
  GPIO_PORTA_DEN_R   |= 0x03;

  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011;
}

void UART1_Init(void) {
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
  while (~SYSCTL_PRUART_R & SYSCTL_PRUART_R1) {}
  while (~SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) {}

  // disable while configuring
  UART1_CTL_R = 0;                 
  
  // // 115200 baud at 16 MHz
  // UART1_IBRD_R = 8;                        
  // UART1_FBRD_R = 44;

  // 2Mbps baud at 16 MHz
  UART1_IBRD_R = 1;                        
  UART1_FBRD_R = 0;


  // 8 data bits, no parity, 1 stop
  UART1_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;          
  // UART1_CTL_R = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;
  UART1_CTL_R = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE | UART_CTL_HSE;


  // set the UART1 Receive Interrupt Mask and Receive Timeout Mask
  UART1_IM_R |= 0x50; 

  // set the UART1 in NVIC Interrupt Set Enable 
  NVIC_EN0_R |= 1 << NVIC_UART1;
  
  // PB0 (RX), PB1 (TX)
  GPIO_PORTB_AFSEL_R |= 0x03;               
  GPIO_PORTB_DEN_R   |= 0x03;

  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) | 0x00000011;
}


void UART1_PutByte(char c) {
  while (UART1_FR_R & UART_FR_TXFF) {}      // wait while transmit FIFO is full
  UART1_DR_R = c;
}

int UART1_ReadByte(uint8_t* byte) {
  if (UART1_FR_R & UART_FR_RXFE) {
    return -1;
  }      
  *byte = (uint8_t)UART1_DR_R;
  return 0;
}


void UART1_RX_Clear() {
  UART1_ICR_R |= 0x50; 
}

void UART0_PutByte(char c) {
  while (UART0_FR_R & UART_FR_TXFF) {}      // wait while transmit FIFO is full
  UART0_DR_R = c;
}

void UART0_Print(const char *s) {
  while (*s) UART0_PutByte(*s++);
}
