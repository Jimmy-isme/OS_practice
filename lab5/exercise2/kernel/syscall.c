#include "header/syscall.h"
#include "header/thread.h"
#include "header/scheduler.h"
#include "header/uart.h"

void syscall_handler(uint64_t syscall_num, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
    switch (syscall_num) {
        case SYS_GETPID:
            uart_async_puts("Syscall: getpid()\n");
            current_thread()->id;
            break;
        case SYS_UART_READ:
            uart_async_puts("Syscall: uart_read()\n");
            uart_read((char*) arg0, arg1);
            break;
        case SYS_UART_WRITE:
            uart_async_puts("Syscall: uart_write()\n");
            uart_write((const char*) arg0, arg1);
            break;
        case SYS_EXEC:
            uart_async_puts("Syscall: exec()\n");
            exec((const char*) arg0, (char* const*) arg1);
            break;
        case SYS_FORK:
            uart_async_puts("Syscall: fork()\n");
            fork();
            break;
        case SYS_EXIT:
            uart_async_puts("Syscall: exit()\n");
            exit();
            break;
        case SYS_MBOX_CALL:
            uart_async_puts("Syscall: mbox_call()\n");
            mbox_call(arg0, (unsigned int*) arg1);
            break;
        case SYS_KILL:
            uart_async_puts("Syscall: kill()\n");
            kill(arg0);
            break;
        default:
            uart_async_puts("Unknown syscall!\n");
    }
}

