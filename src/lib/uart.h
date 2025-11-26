#ifndef UART_H
#define UART_H

#define BAUD 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD * 16UL))) - 1)

void UART_init(int prescale);

void UART_send( unsigned char data);

void UART_putstring(char* StringPtr);

#endif 