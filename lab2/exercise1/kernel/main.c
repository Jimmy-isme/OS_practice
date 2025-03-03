#include "header/uart.h"
#include "header/shell.h"
#include "header/reboot.h"

unsigned int boot_count=0;

void main() {
    uart_init();  // 初始化 Mini UART

    boot_count++;
    uart_puts("Boot count: ");
    uart_send_hex(boot_count);
    uart_puts("\n");
    
    uart_puts("Wellcome!\n");
    shell_start();  // 啟動 Shell
}

