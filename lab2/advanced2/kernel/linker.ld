SECTIONS
{
    /* 記憶體起始位址 */
    . = 0x80000;

    /* 程式碼段 (Text Section) */
    .text : { 
        KEEP(*(.text.boot)) 
        *(.text .text.* .gnu.linkonce.t*) 
    }

    /* 唯讀資料段 (Read-Only Data Section) */
    .rodata : { 
        *(.rodata .rodata.* .gnu.linkonce.r*) 
    }

    /* 資料段 (Data Section) */
    .data : { 
        . = ALIGN(16);
        *(.data .data.* .gnu.linkonce.d*) 
    }

    /* BSS 段 (未初始化變數，需清零) */
    .bss (NOLOAD) : { 
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }

    /* 堆疊空間 (Stack) */
    . = ALIGN(16);
    __stack_top = . + 0x4000;  /* 16 KB 堆疊 */

    /* 丟棄不必要的段 */
    /DISCARD/ : { 
        *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) 
    }
}

/* 計算 BSS 大小 */
    __bss_size = (__bss_end - __bss_start) >> 3;

