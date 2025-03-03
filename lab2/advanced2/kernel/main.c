#include "header/uart.h"
#include "header/shell.h"
#include "header/dtb.h"

extern void *_dtb_ptr;

void main() {
    uart_init();  // 初始化 Mini UART

    fdt_traverse(get_cpio_addr,_dtb_ptr);
    uart_puts("Type in `help` to get instruction menu!\n");

    shell_start();  // 啟動 Shell
}

