#include "header/uart.h"
#include "header/shell.h"
#include "header/dtb.h"
#include "header/cpio.h"
#include "header/timer.h"
#include "header/allocator.h"

int main() {
    uart_init();
    init_allocator();

    void *p1 = dynamic_malloc(20);  // 20B -> 32B chunk
    void *p2 = dynamic_malloc(50);  // 50B -> 96B chunk
    void *p3 = dynamic_malloc(300); // 300B -> 512B chunk
    print_memory_state();

    dynamic_free(p1, 20);
    dynamic_free(p2, 50);
    dynamic_free(p3, 300);
    print_memory_state();
    
    shell_start();
}
