.global _start  // 程式的進入點

.section .text
_start:
    // 1. 存儲 Device Tree Blob (DTB) 位址
    ldr x1, =_dtb_ptr  // 取得_dtb_ptr位址
    str x0, [x1]       // _dtb_ptr位址指向的值存入 x0

    // 2. 判斷 CPU ID，僅允許 CPU 0 繼續執行
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, init        // 如果是 CPU 0，繼續執行
1:  wfe                 // 其他核心進入低功耗等待
    b 1b

init:
    // 3. 設定 Stack Pointer (SP)
    ldr x0, =__stack_top  // 讀取_stack_top位址
    mov sp, x0           // 設定 SP，堆疊從這裡開始往下增長

    // 4. 清除 .bss 段
    ldr x1, =__bss_start  // 取得 .bss 開始位址
    ldr x2, =__bss_end    // 取得 .bss 結束位址
    mov x3, #0           // 設定0作為清零的值

clear_bss:
    cmp x1, x2           // 如果 x1 == x2，表示 .bss 已清空
    b.ge done_clear_bss  // 跳出迴圈
    str x3, [x1], #8     // 將0存入x1指向的位置，然後x1 += 8
    b clear_bss

done_clear_bss:
    // 5. 跳轉到 main()，開始執行 C 程式
    bl main

    // 6. 如果 main結束，就進入死循環 (因為 kernel 沒有 return)
hang:
    wfe  // 等待事件，降低 CPU 能耗
    b hang

.global _dtb_ptr  // 定義 _dtb_ptr
.section .data
_dtb_ptr: .dword 0x0  // _dtb_ptr 初始化為 0

