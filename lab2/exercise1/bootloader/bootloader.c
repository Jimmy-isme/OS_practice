#include "header/bootloader.h"
#include "header/uart.h"

void load_img(){
    unsigned int size = 0;
    unsigned char *size_buffer = (unsigned char *) &size;

    // 等待 kernel 大小
    uart_puts("Waiting for kernel size...\n");
    for(int i=0; i<4; i++)
    {
        size_buffer[i] = uart_recv();
    }
    uart_puts("Size received!\n");

    // 讀取 kernel 並存入 0x80000
    char *kernel = (char *) 0x80000;
    for (unsigned int i = 0; i < size; i++) {
        kernel[i] = uart_recv();

        if (i % 1024 == 0) {  // 每 1KB 顯示進度
            uart_puts(".");
        }
    }

    uart_puts("\nKernel loaded! Jumping to kernel...\n");

    // 跳轉執行 kernel
    void (*kernel_entry)() = (void (*)()) 0x80000;
    kernel_entry();
}

