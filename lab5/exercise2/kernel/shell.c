#include "header/uart.h"
#include "header/shell.h"
#include "header/utils.h"
#include "header/mailbox.h"
#include "header/cpio.h"
#include "header/reboot.h"
#include "header/exec.h"
#include "header/syscall.h"
#include "header/process.h"

#define CMD_MAX_LEN 64

void shell_start() 
{
    char buffer[CMD_MAX_LEN];

    uart_async_puts("Simple Shell v1.0\n");
    uart_async_puts("Type 'help' to see available commands.\n\n");

    while (1) 
    {
        uart_async_puts("> ");  // 顯示提示符號
        int index = 0;

        // **同步讀取使用者輸入 (busy-polling)**
        while (1) 
        {
            char c = uart_async_recv();  // **等待輸入字元**

            if (c == '\r') {  // **Enter 鍵**
                buffer[index] = '\0';  // 字串結尾
                uart_async_puts("\r\n");  // 顯示換行
                break;
            } 
            else if ((c == 8 || c == 127) && index > 0) {  // **Backspace**
                index--;
                uart_async_puts("\b \b");  // **即時刪除字元**
            } 
            else if (index < CMD_MAX_LEN - 1) {  // **正常字元**
                buffer[index++] = c;
                uart_async_send(c);  // **回顯輸入字元**
            }
        }

        // **執行指令**
        if (index == 0) continue;  // **如果沒輸入指令，跳過**

        if (string_compare(buffer, "help") == 0) {
            uart_async_puts("Available commands:\n");
            uart_async_puts("  help  - Show available commands\n");
            uart_async_puts("  hello - Print 'Hello World!'\n");
            uart_async_puts("  info - Get the hardware's information\n");
            uart_async_puts("  reboot - Reboot machine\n");
            uart_async_puts("  ls - list files\n");
            uart_async_puts("  cat - read files\n");
            uart_async_puts("  exec - execute files\n");
            //uart_async_puts("  timer - Start periodic timer interrupt (prints uptime)\n");
        } 
        else if (string_compare(buffer, "hello") == 0) {
            uart_async_puts("Hello World!\n");
        }
        else if (string_compare(buffer, "info") == 0) {
            uart_async_puts("System Information:\n");

            unsigned int mailbox[8] __attribute__((aligned(16)));

            // 取得主機板修訂版
            mailbox[0] = 7 * 4;
            mailbox[1] = 0;
            mailbox[2] = TAG_GET_BOARD_REVISION;
            mailbox[3] = 4;
            mailbox[4] = 0;
            mailbox[5] = 0;
            mailbox[6] = 0;

            if (mailbox_call(mailbox)) {
                uart_async_puts("Board Revision: 0x");
                uart_async_send_hex(mailbox[5]);
                uart_async_puts("\n");
            } 
            else {
                uart_async_puts("Failed to get board revision\n");
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

            if (mailbox_call(mailbox)) {
                uart_async_puts("Memory Base Address: 0x");
                uart_async_send_hex(mailbox[5]);
                uart_async_puts("\n");

                uart_async_puts("Memory Size: 0x");
                uart_async_send_hex(mailbox[6]);
                uart_async_puts("\n");
            } 
            else {
                uart_async_puts("Failed to get memory info\n");
            }
        }
        else if (string_compare(buffer, "ls") == 0) {
            cpio_ls();  // 列出 initramfs 內的檔案
        }
        else if (string_starts_with(buffer, "cat ")) {
            char *filename = buffer + 4;  // 取得 `cat <filename>` 的 `filename`
            cpio_cat(filename);
        }
        else if (string_starts_with(buffer, "exec ")) {
            exec_user(buffer + 5);  // 執行 `exec <filename>`
        }

        else if (string_compare(buffer, "reboot") == 0) {
            uart_async_puts("Rebooting....\n");
            reset(1000);
        }
        else {
            uart_async_puts("Command not found.\n");
        }
    }
}
