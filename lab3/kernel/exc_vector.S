.global exception_vector_table  // 讓其他程式可以找到異常向量表
.global exception_handler       // 讓異常處理函數對外可見
.global exception_entry         // 讓 .C 的 exception_entry 可以被使用

// 儲存所有通用暫存器
.macro save_all
    sub sp, sp, 32 * 8
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    str x30, [sp, 16 * 15]
.endm

// 恢復所有通用暫存器
.macro load_all
    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldr x30, [sp, 16 * 15]
    add sp, sp, 32 * 8
.endm

// 定義異常向量表 (Vector Table) (SP0跟AArch32不用，只用SPx跟AArch64)
.align 11
exception_vector_table:

  // SP0
  b exception_handler    // branch to a handler function.
  .align 7               // entry size is 0x80, .align will pad 0
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  // SPx
  b exception_handler
  .align 7
  b irq_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  // AArch64
  b exception_handler
  .align 7
  b irq_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  // AArch32
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

// 異常處理函數
exception_handler:
    save_all             // 儲存暫存器
    bl exception_entry   // 呼叫 C 語言函數處理異常 (由 exception.c 實作)
    load_all             // 恢復暫存器
    eret                 // 返回異常發生前的位置
    


#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define IRQ_PENDING_1        0x3f00b204
#define AUX_MU_IIR_REG       0x3f215048

// **中斷處理函式**
irq_handler:
    save_all

    // 讀取 Core Timer IRQ 狀態
    ldr x0, =CORE0_TIMER_IRQ_CTRL
    ldr w1, [x0]
    
    // 檢查是否為 Core Timer IRQ（位元 1）
    and w1, w1, #2
    cbz w1, check_uart
    bl core_timer_handler
    
    b irq_done
check_uart:
    ldr x0, =IRQ_PENDING_1
    ldr w1, [x0]
    tst w1, #(1 << 29)     // 檢查 mini UART 中斷 (bit 29)
    beq irq_done
    ldr x0, =AUX_MU_IIR_REG
    ldr w1, [x0]
    and w2, w1, #6         // 檢查中斷類型 (bit 1-2)
    cmp w2, #4             // RX 中斷 (IIR = 0b100)
    beq uart_rx
    cmp w2, #2             // TX 中斷 (IIR = 0b010)
    beq uart_tx
    b irq_done
uart_rx:
    bl uart_rx_handler
    b irq_done
uart_tx:
    bl uart_tx_handler
    b irq_done
irq_done:
    load_all
    eret

