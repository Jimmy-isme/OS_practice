.global _start  // 程式的進入點 (bootloader 會從這裡開始執行)

.section .text
_start:
    // 1. 設定 Stack Pointer (SP)
    ldr x0, =_stack_top  // 讀取 _stack_top 位址
    mov sp, x0           // 設定 SP，堆疊從這裡開始往下增長

    // 2. 清除 `.bss` 段，確保未初始化的變數是 0
    ldr x1, =_bss_start  // 取得 .bss 開始位址
    ldr x2, =_bss_end    // 取得 .bss 結束位址
    mov x3, #0           // 設定0作為清零的值

clear_bss:
    cmp x1, x2           // 如果 x1 == x2，表示 .bss 已清空
    b.ge done_clear_bss  // 跳出迴圈
    str x3, [x1], #8     // 將 0 存入 x1 指向的位置，然後 x1 += 8
    b clear_bss

done_clear_bss:
    // 3. 跳到 main
    bl main

    // 4. 如果 main() 結束，就進入死循環 (因為 kernel 沒有 return)
hang:
    wfe  // 等待事件，降低 CPU 能耗
    b hang

