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

    uart_puts("[Exception] Returning to normal execution...\n");
}

