#include "gpio.h"

// 設定 GPIO 功能 (ex: UART、輸入、輸出)
void gpio_set_function(int pin, int function) {
    volatile unsigned int *reg = GPFSEL1;  // 只處理 GPIO 14, 15 (UART)
    int shift = (pin % 10) * 3;
    *reg &= ~(7 << shift);   // 先清除原本的設定
    *reg |= (function << shift);  // 設定新功能
}

// 設定 GPIO 電位 (高/低)
void gpio_write(int pin, int value) {
    if (value)
        *GPSET0 = (1 << pin);
    else
        *GPCLR0 = (1 << pin);
}

// 延遲 (單純的忙等待)
void delay(int cycles) {
    while (cycles-- > 0) asm volatile("nop");
}

