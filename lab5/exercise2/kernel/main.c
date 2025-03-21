#include "header/uart.h"
#include "header/shell.h"
#include "header/dtb.h"
#include "header/cpio.h"
#include "header/timer.h"
#include "header/allocator.h"
#include "header/thread.h"
#include "header/scheduler.h" 
#include "header/idle.h"
#include "header/thread.h"
#include "header/syscall.h"

void fork_test() {
    uart_async_puts("\nFork Test, pid: ");
    uart_send_hex(getpid());
    uart_async_puts("\n");

    int cnt = 1;
    int ret = fork();
    
    if (ret == 0) {  // **子行程**
        long long cur_sp;
        asm volatile("mov %0, sp" : "=r"(cur_sp));

        uart_async_puts("First child pid: ");
        uart_send_hex(getpid());
        uart_async_puts(", cnt: ");
        uart_send_hex(cnt);
        uart_async_puts(", ptr: ");
        uart_send_hex((unsigned long)&cnt);
        uart_async_puts(", sp: ");
        uart_send_hex(cur_sp);
        uart_async_puts("\n");

        ++cnt;
        
        if (fork() != 0) {  // **第一個子行程再創建子行程**
            asm volatile("mov %0, sp" : "=r"(cur_sp));
            uart_async_puts("First child pid: ");
            uart_send_hex(getpid());
            uart_async_puts(", cnt: ");
            uart_send_hex(cnt);
            uart_async_puts(", ptr: ");
            uart_send_hex((unsigned long)&cnt);
            uart_async_puts(", sp: ");
            uart_send_hex(cur_sp);
            uart_async_puts("\n");
        } else {  // **第二個子行程**
            while (cnt < 5) {
                asm volatile("mov %0, sp" : "=r"(cur_sp));
                uart_async_puts("Second child pid: ");
                uart_send_hex(getpid());
                uart_async_puts(", cnt: ");
                uart_send_hex(cnt);
                uart_async_puts(", ptr: ");
                uart_send_hex((unsigned long)&cnt);
                uart_async_puts(", sp: ");
                uart_send_hex(cur_sp);
                uart_async_puts("\n");

                delay(1000000);
                ++cnt;
            }
        }
        exit();
    } else {  // **父行程**
        uart_async_puts("Parent here, pid: ");
        uart_send_hex(getpid());
        uart_async_puts(", child pid: ");
        uart_send_hex(ret);
        uart_async_puts("\n");
    }
}

#include "uart.h"
#include "syscall.h"
#include "shell.h"
#include "fork_test.h"

int main() {
    uart_init();
    init_allocator();

    uart_async_puts("Starting fork test...\n");
    fork_test();
    
    uart_async_puts("Fork test completed. Entering shell...\n");
    shell_start();
}
