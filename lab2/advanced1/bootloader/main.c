#include "header/uart.h"
#include "header/bootloader.h"

void main()
{
    // set up serial console
    uart_init();
    // say hello
	int s=500;
	while(s--){
		asm volatile("nop");
	}
    uart_puts("Start Bootloading\n");
	load_img();
}
