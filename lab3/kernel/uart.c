#include "header/uart.h"
#include "header/gpio.h"
#include <stdint.h>

#define AUX_ENABLES     ((volatile unsigned int*)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO_REG   ((volatile unsigned int*)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER_REG  ((volatile unsigned int*)(MMIO_BASE + 0x00215044))
#define AUX_MU_LCR_REG  ((volatile unsigned int*)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR_REG  ((volatile unsigned int*)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR_REG  ((volatile unsigned int*)(MMIO_BASE + 0x00215054))
#define AUX_MU_CNTL_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215060))
#define AUX_MU_BAUD_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215068))
#define AUX_MU_IIR_REG  ((volatile unsigned int*)(MMIO_BASE + 0x00215048))

#define ENABLE_IRQS1    ((volatile unsigned int*)(MMIO_BASE + 0x0000b210))

#define BUFFER_SIZE 256

// 讀緩衝區
volatile char rx_buffer[BUFFER_SIZE];
volatile int rx_head = 0;
volatile int rx_tail = 0;

// 寫緩衝區 
volatile char tx_buffer[BUFFER_SIZE];
volatile int tx_head = 0;
volatile int tx_tail = 0;

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
    
    
    // 9. 啟用 mini UART 中斷（RX 和 TX）
    *AUX_MU_IER_REG = 3;  // bit 0 = 1 (RX 中斷), bit 1 = 1 (TX 中斷)

    // 10. 啟用第二級中斷控制器的 bit 29 (mini UART IRQ)
    *ENABLE_IRQS1 |= (1 << 29);

    // 11. 確保 CPU 接受 IRQ (清除 DAIF 的 IRQ 遮罩)
    asm volatile("msr daifclr, #2");
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

// **非同步功能（用於 Shell 互動)**
void uart_rx_handler() {
    while (*AUX_MU_LSR_REG & 0x01) {  // 檢查是否有資料可讀
        uint8_t c = *AUX_MU_IO_REG;   // 讀取資料
        uint32_t next_head = (rx_head + 1) % BUFFER_SIZE;
        if (next_head != rx_tail) {   // 避免緩衝區溢位
            rx_buffer[rx_head] = c;
            rx_head = next_head;
        }
    }
    
    *AUX_MU_IIR_REG = 0xC6;
}

// 非同步發送處理函數
void uart_tx_handler() {
    while (tx_head != tx_tail && (*AUX_MU_LSR_REG & 0x20)) {  // 檢查緩衝區有資料且 TX 可用
        *AUX_MU_IO_REG = tx_buffer[tx_tail];  // 發送資料
        tx_tail = (tx_tail + 1) % BUFFER_SIZE;
    }
    if (tx_head == tx_tail) {  // 緩衝區已清空，禁用 TX 中斷
        *AUX_MU_IER_REG &= ~2;  // 清除 bit 1
        
        //uart_puts("[DEBUG] TX interrupt disabled\n");
    }
    
    *AUX_MU_IIR_REG = 0xC6;
}

// 非同步接收（從緩衝區讀取）
char uart_async_recv() {
    while (rx_head == rx_tail);  // 等待資料（可改進為非阻塞）
    char c = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % BUFFER_SIZE;
    return c;
}

// 非同步發送（寫入緩衝區）
void uart_async_send(unsigned int c) {
    uint32_t next_head = (tx_head + 1) % BUFFER_SIZE;
    while (next_head == tx_tail);  // 等待緩衝區空間（可改進為非阻塞）
    tx_buffer[tx_head] = c;
    tx_head = next_head;
    *AUX_MU_IER_REG |= 2;  // 啟用 TX 中斷 (bit 1 = 1)
}

// 非同步發送字串
void uart_async_puts(char *s) {
    while (*s) {
        if (*s == '\n') uart_async_send('\r');
        uart_async_send(*s++);
    }
}

void uart_async_send_hex(unsigned int value) {
    char hex[9];
    for (int i = 7; i >= 0; i--) {
        int nibble = (value >> (i * 4)) & 0xF;
        hex[7 - i] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
    }
    hex[8] = '\0';
    uart_async_puts(hex);
}



