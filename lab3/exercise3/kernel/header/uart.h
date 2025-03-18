#ifndef _UART_H_
#define _UART_H_

void uart_init();
void uart_send(unsigned int c);
char uart_recv();
void uart_puts(char *s);
void uart_send_hex(unsigned int value);
void uart_rx_handler();
void uart_tx_handler();
char uart_async_recv();
void uart_async_send(unsigned int c);
void uart_async_puts(char *s);
void uart_async_send_hex(unsigned int value);

#endif

