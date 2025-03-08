#include "header/timer.h"
#include "header/uart.h"


void core_timer_enable() {
    unsigned long cntfrq;
    
    // 啟用 core timer
    asm volatile("mov x0, 1; msr cntp_ctl_el0, x0");

    // 讀取 timer 頻率
    asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq));

    // 設定 2 秒後觸發中斷
    asm volatile("msr cntp_tval_el0, %0" :: "r"(cntfrq * 2));

    // 解除中斷屏蔽
    *CORE0_TIMER_IRQ_CTRL = 2; // 觸發中斷

    // **開啟 IRQ**
    asm volatile("msr daifclr, #2");
}

// Timer 中斷處理函式
void core_timer_handler() {
    unsigned long cntfrq, cntpct, seconds;

    // 讀取當前時間
    asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq));
    asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct));

    // 計算開機後的秒數
    seconds = cntpct / cntfrq;

    // 輸出時間資訊
    uart_puts("[Timer Interrupt] Uptime: ");
    uart_send_hex(seconds);
    uart_puts(" seconds\n");

    // 設定下一次中斷 (2 秒後)
    asm volatile("msr cntp_tval_el0, %0" :: "r"(cntfrq * 2));

    // **清除 Timer IRQ 旗標**
    *CORE0_TIMER_IRQ_CTRL = 2;  // 關閉中斷(***待辦)
}

