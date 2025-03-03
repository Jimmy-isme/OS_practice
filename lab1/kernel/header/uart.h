#ifndef _UART_H_
#define _UART_H_

void uart_init();
void uart_send(unsigned int c);
char uart_recv();
void uart_puts(char *s);
void uart_send_hex(unsigned int value);

#endif

