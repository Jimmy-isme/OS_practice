#ifndef _TIMER_H_
#define _TIMER_H_

#define CORE0_TIMER_IRQ_CTRL ((volatile unsigned int*) 0x40000040)  // Core 0 Timer IRQ 控制暫存器

void core_timer_enable();   // 啟用 Core Timer
void core_timer_handler();  // 處理 Timer 中斷

#endif

