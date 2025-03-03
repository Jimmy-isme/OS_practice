#include "header/uart.h"
#include "header/shell.h"
#include "header/utils.h"
#include "header/mailbox.h"
#include "header/reboot.h"

#define CMD_MAX_LEN 64

void shell_start() 
{
    char buffer[CMD_MAX_LEN];

    uart_puts("Simple Shell v1.0\n");
    uart_puts("Type 'help' to see available commands.\n\n");

    while (1) 
    {
        uart_puts("> ");  // 顯示提示符號
        int index = 0;

        // 讀取使用者輸入，直到 Enter (`\r`) 被按下
        while (1) 
        {
            char c = uart_recv();
            if (c == '\r') 
            {  // Enter 鍵
                buffer[index] = '\0';  // 字串結尾
                uart_puts("\r\n");  // 顯示換行
                break;
            } 
            else if (c == '\b' && index > 0) 
            {  // 處理 Backspace
                index--;
                uart_puts("\b \b");  // 刪除字元
            } 
            else if (index < CMD_MAX_LEN - 1) 
            {  // 普通字元
                buffer[index++] = c;
                uart_send(c);  // 顯示輸入字元
            }
        }

        // 執行指令
        if (index == 0) continue;  // 如果沒輸入指令，跳過

        if (string_compare(buffer, "help") == 0) 
        {
            uart_puts("Available commands:\n");
            uart_puts("  help  - Show available commands\n");
            uart_puts("  hello - Print 'Hello World!'\n");
            uart_puts("  info - Get the hardware's information\n");
            uart_puts("  reboot - Reboot machine\n");
        } 
        
        else if (string_compare(buffer, "hello") == 0) 
        {
            uart_puts("Hello World!\n");
        }
        
        else if (string_compare(buffer, "info") == 0) 
        {
	    uart_puts("System Information:\n");

	    unsigned int mailbox[8] __attribute__((aligned(16)));

	    // 取得主機板修訂版
	    mailbox[0] = 7 * 4;
	    mailbox[1] = 0;
	    mailbox[2] = TAG_GET_BOARD_REVISION;
	    mailbox[3] = 4;
	    mailbox[4] = 0;
	    mailbox[5] = 0;
	    mailbox[6] = 0;

	    if (mailbox_call(mailbox)) 
	    {
		uart_puts("Board Revision: 0x");
		uart_send_hex(mailbox[5]);
		uart_puts("\n");
	    } 
	    else 
	    {
		uart_puts("Failed to get board revision\n");
	    }

	    // 取得記憶體大小
	    mailbox[0] = 8 * 4;
	    mailbox[1] = 0;
	    mailbox[2] = TAG_GET_MEMORY;
	    mailbox[3] = 8;
	    mailbox[4] = 0;
	    mailbox[5] = 0;
	    mailbox[6] = 0;
	    mailbox[7] = 0;

	    if (mailbox_call(mailbox)) 
	    {
		uart_puts("Memory Base Address: 0x");
		uart_send_hex(mailbox[5]);
		uart_puts("\n");

		uart_puts("Memory Size: 0x");
		uart_send_hex(mailbox[6]);
		uart_puts("\n");
	    } 
	    
	    else 
	    {
		uart_puts("Failed to get memory info\n");
	    }
		
        }
        else if (string_compare(buffer, "reboot") == 0) 
        {
            uart_puts("Rebooting....\n");
            reset(1000);
        }
        else 
        {
            uart_puts("Command not found.\n");
        }
    }
}

