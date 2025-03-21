#include "header/exception.h"
#include "header/uart.h"

void exception_entry() {
    uart_puts("[Exception] CPU entered exception handler!\n");

    unsigned long spsr_el1, elr_el1, esr_el1;
    asm volatile (
        "mrs %0, spsr_el1\n"
        "mrs %1, elr_el1\n"
        "mrs %2, esr_el1\n"
        : "=r"(spsr_el1), "=r"(elr_el1), "=r"(esr_el1)
    );

    uart_puts("SPSR_EL1: ");
    uart_send_hex(spsr_el1);
    uart_puts("\nELR_EL1: ");
    uart_send_hex(elr_el1);
    uart_puts("\nESR_EL1: ");
    uart_send_hex(esr_el1);
    uart_puts("\n");

    // **檢查是否是 SVC (系統呼叫)**
    if ((esr_el1 >> 26) == 0b010101) {  // **ESR_EL1[31:26] == 0b010101 表示 SVC**
        unsigned long syscall_num;
        asm volatile ("mov %0, x8" : "=r"(syscall_num));  // **取得 SVC 編號（存於 x8）**

        uart_puts("[Syscall] Handling syscall: ");
        uart_send_hex(syscall_num);
        uart_puts("\n");

        // **呼叫 syscall_handler()，傳遞 x8（syscall_num），以及 x0-x2（參數）**
        syscall_handler(syscall_num, 0, 0, 0);

        uart_puts("[Syscall] Returning to user mode...\n");
    } else {
        uart_puts("[Exception] Unhandled exception!\n");
    }

    uart_puts("[Exception] Returning to normal execution...\n");
}

