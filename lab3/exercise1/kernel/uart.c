#include "header/uart.h"
#include "header/gpio.h"

#define AUX_ENABLES     ((volatile unsigned int*)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO_REG   ((volatile unsigned int*)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER_REG  ((volatile unsigned int*)(MMIO_BASE + 0x00215044))
#define AUX_MU_LCR_REG  ((volatile unsigned int*)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR_REG  ((volatile unsigned int*)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR_REG  ((volatile unsigned int*)(MMIO_BASE + 0x00215054))
#define AUX_MU_CNTL_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215060))
#define AUX_MU_BAUD_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215068))

void uart_init() {
    // 1. 啟用 Mini UART
    *AUX_ENABLES |= 1;  

    // 2. 禁用 UART 傳輸
    *AUX_MU_CNTL_REG = 0;

    // 3. 設定 8-bit 模式
    *AUX_MU_LCR_REG = 3;

    // 4. 禁用 RTS
    *AUX_MU_MCR_REG = 0;

    // 5. 設定波特率 115200 (假設 core_freq = 250MHz)
    *AUX_MU_BAUD_REG = 270;

    // 6. 設定 GPIO 14 (TX) 和 GPIO 15 (RX) 為 ALT5 (Mini UART)
    gpio_set_function(14, 2); // TX
    gpio_set_function(15, 2); // RX

    // 7. 清除 GPIO pull-up/down
    *GPPUD = 0;
    delay(150);
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    delay(150);
    *GPPUDCLK0 = 0;

    // 8. 啟用 UART 傳輸
    *AUX_MU_CNTL_REG = 3;
}

// 發送單一字元
void uart_send(unsigned int c) {
    while (!(*AUX_MU_LSR_REG & 0x20));  // 等待 TX 可用
    *AUX_MU_IO_REG = c;
}

// 接收單一字元
char uart_recv() {
    while (!(*AUX_MU_LSR_REG & 0x01));  // 等待 RX 有資料
    return (char)(*AUX_MU_IO_REG);
}

// 發送字串
void uart_puts(char *s) {
    while (*s) {
        if (*s == '\n') uart_send('\r');  // 確保 \n 轉換為 \r\n
        uart_send(*s++);
    }
}

// mailbox 
void uart_send_hex(unsigned int value) {
    char hex[9];
    for (int i = 7; i >= 0; i--) {
        int nibble = (value >> (i * 4)) & 0xF;
        hex[7 - i] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
    }
    hex[8] = '\0';
    uart_puts(hex);
}

