.global _start
.section .text

_start:
    ldr x0, =_stack_top
    mov sp, x0

    // 清除 .bss 段
    ldr x1, =_bss_start
    ldr x2, =_bss_end
    mov x3, #0

clear_bss:
    cmp x1, x2
    b.ge done_clear_bss
    str x3, [x1], #8
    b clear_bss

done_clear_bss:
    // 跳轉到 main
    bl main

hang:
    wfe
    b hang

