SECTIONS {
    . = 0x60000;  /* Bootloader 入口點 */

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .bss (NOLOAD) : {
        _bss_start = .;
        *(.bss)
        _bss_end = .;
    }

    /* 設定堆疊空間 (Stack) */
    . = ALIGN(16);
    _stack_top = . + 0x4000;  /* 16 KB 堆疊 */
}
