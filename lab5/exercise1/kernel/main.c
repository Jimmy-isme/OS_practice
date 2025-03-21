#include "header/uart.h"
#include "header/shell.h"
#include "header/dtb.h"
#include "header/cpio.h"
#include "header/timer.h"
#include "header/allocator.h"
#include "header/thread.h"
#include "header/scheduler.h" 
#include "header/idle.h"

void foo() {
    for (int i = 0; i < 5; i++) {
        uart_async_puts("Thread ID: ");
        uart_send_hex(current_thread()->id);  // **用 16 進位顯示 ID**
        uart_async_puts(" Iteration: ");
        uart_send_hex(i);  // **用 16 進位顯示迴圈次數**
        uart_async_puts("\n");

        schedule();  // **讓出 CPU**
    }
    thread_exit();  // **執行緒結束**
}

int main() {

    uart_init();
    init_allocator();
    
    for (int i = 0; i < 3; i++) {
        thread_create(foo);  // **建立 3 個執行緒**
    }
    
    shell_start();
}
