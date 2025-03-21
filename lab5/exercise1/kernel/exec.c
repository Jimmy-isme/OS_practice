#include "header/exec.h"
#include "header/uart.h"
#include "header/utils.h"
#include "header/cpio.h"
#include "header/allocator.h"

void exec_user(char* filename) {
    char* file_addr = cpio_findFileAddress(filename);

    if (file_addr == 0) {
        uart_puts("Error: File not found!\n");
        return;
    }

    uart_puts("Loading user program...\n");

    // 分配記憶體給 User Program
    void* exec_addr = buddy_malloc(cpio_get_file_size(filename));

    // 複製 User Program 到分配的記憶體
    memcpy(exec_addr, file_addr, cpio_get_file_size(filename));

    // 分配 User Stack
    unsigned long sp = (unsigned long) buddy_malloc(4096);

    // 設定 CPU 狀態並跳轉執行
    asm volatile (
        "msr spsr_el1, %0\n"
        "msr elr_el1, %1\n"
        "msr sp_el0, %2\n"
        "eret\n"
        :
        : "r"(0x3c0), "r"(exec_addr), "r"(sp)
        : "memory"
    );
}


