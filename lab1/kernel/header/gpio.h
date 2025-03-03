#ifndef _GPIO_H_
#define _GPIO_H_

#define MMIO_BASE 0x3F000000  // 樹莓派 3 的 MMIO 基址

// GPIO
#define GPFSEL1     ((volatile unsigned int*)(MMIO_BASE + 0x00200004))
#define GPSET0      ((volatile unsigned int*)(MMIO_BASE + 0x0020001C))
#define GPCLR0      ((volatile unsigned int*)(MMIO_BASE + 0x00200028))
#define GPPUD       ((volatile unsigned int*)(MMIO_BASE + 0x00200094))
#define GPPUDCLK0   ((volatile unsigned int*)(MMIO_BASE + 0x00200098))

// 簡單的 GPIO 操作函式
void gpio_set_function(int pin, int function);
void gpio_write(int pin, int value);
void delay(int cycles);

#endif

