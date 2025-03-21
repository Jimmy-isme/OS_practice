.global _start

.section .text.boot
_start:
    // 存儲 Device Tree Blob (DTB) 位址
    ldr x1, =_dtb_ptr  
    str x0, [x1]       

    // 優化! 判斷 CPU ID，僅允許 CPU 0 繼續執行
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, cpu0        // 如果是 CPU 0，繼續執行
1:  wfe                 // 其他核心進入低功耗等待
    b 1b

cpu0:
    // 以 _start 的位址作為堆疊基底，但先進行 EL2→EL1 切換
    ldr     x1, =_start // 優化! 將 stack 起點設置在 0x80000
    bl      from_el2_to_el1
    mov     sp, x1      // 切換回來後重新設定堆疊

    // **新增：初始化 tpidr_el1，確保執行緒機制正常**
    mov x0, #0
    msr tpidr_el1, x0   // 設定當前執行緒指標

    // 設定異常向量表
set_exception_vector_table:
    adr     x0, exception_vector_table
    msr     vbar_el1, x0

    // 清除 .bss 區段（從 __bss_start 開始，大小為 __bss_size * 8 bytes）
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
    
clear_bss:
    cbz     w2, done_clear_bss
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, clear_bss
    
done_clear_bss:

    bl      main

hang:
    b       hang

// EL2 -> EL1 切換程式碼：依照作業要求設定 hcr_el2, spsr_el2, elr_el2 並用 eret 返回 EL1
from_el2_to_el1:
    mov     x0, (1 << 31)   // 設定 EL1 為 AARCH64 執行模式
    msr     hcr_el2, x0
    mov     x0, 0x3c5       // 設定 EL1h 模式 (關閉中斷)
    msr     spsr_el2, x0
    msr     elr_el2, lr     // 返回地址設定為呼叫者的 lr
    eret                  // 切換到 EL1

// 全域變數 _dtb_ptr 的定義（存放 DTB 位址）
.global _dtb_ptr
.section .data
_dtb_ptr: .dc.a 0x0

