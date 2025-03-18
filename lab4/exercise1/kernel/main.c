#include "header/uart.h"
#include "header/shell.h"
#include "header/dtb.h"
#include "header/cpio.h"
#include "header/timer.h"
#include "header/allocator.h"

int main() {
    uart_init();
    init_allocator();
    print_memory_state();

    void *p1 = buddy_malloc(8000);
    void *p2 = buddy_malloc(4000);
    print_memory_state();

    buddy_free(p1);
    buddy_free(p2);
    print_memory_state();
    
    shell_start();
}
