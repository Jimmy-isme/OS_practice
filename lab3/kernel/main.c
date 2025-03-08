#include "header/uart.h"
#include "header/shell.h"
#include "header/dtb.h"
#include "header/cpio.h"
#include "header/timer.h"

//extern void *_dtb_ptr;
void main()
{
    uart_init();
    uart_async_puts("Test async-uart!\n");
	
    //fdt_traverse(get_cpio_addr,_dtb_ptr);
    
    // Exercise 2
    uart_async_puts("Enabling core timer interrupt...\n");
    core_timer_enable();
    
    uart_async_puts("Type in `help` to get instruction menu!\n");
    
    shell_start();
}
