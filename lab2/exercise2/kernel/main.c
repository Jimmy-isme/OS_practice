#include "header/uart.h"
#include "header/shell.h"
#include "header/reboot.h"
#include "header/cpio.h"

unsigned int boot_count = 0;

void main() {
    uart_init();  // 初始化 Mini UART

    boot_count++;
    uart_puts("Boot count: "); // Test if it really reboots
    uart_send_hex(boot_count);
    uart_puts("\n");

    uart_puts("Welcome!\n");  // ✅ 加入 Hello World 招呼語

    // 解析 initramfs 並讀取 test.txt
    uart_puts("Parsing initramfs...\n");
    cpio_ls();

    shell_start();  // 啟動 Shell
}

