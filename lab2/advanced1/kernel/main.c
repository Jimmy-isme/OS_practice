#include "header/uart.h"
#include "header/shell.h"
#include "header/allocator.h"

unsigned int boot_count = 0;

void main() {
    uart_init();  // 初始化 Mini UART

    uart_puts("Welcome!\n");  // 加入 Hello World 招呼語

    
    // Exercise 3
    char *ptr1 = (char*) simple_malloc(16);
    char *ptr2 = (char*) simple_malloc(32);
    
    uart_puts("Allocated memory:\n");
    uart_send_hex((unsigned long) ptr1);
    uart_puts("\n");
    uart_send_hex((unsigned long) ptr2);
    uart_puts("\n");

    shell_start();  // 啟動 Shell
}

