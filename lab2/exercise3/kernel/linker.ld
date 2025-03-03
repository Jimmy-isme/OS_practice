SECTIONS
{
    /* 記憶體起始位址 */
    . = 0x80000;

    /* 程式碼段 (Text Section) */
    .text : {
        *(.text)
    }

    /* 資料段 (Data Section) */
    .data : {
        *(.data)
    }

    /* BSS 段 (未初始化變數，需清零) */
    .bss : {
        _bss_start = .;
        *(.bss)
        *(COMMON)
        _bss_end = .;
    }

    /* 堆疊空間 (Stack) */
    . = ALIGN(16);
    _stack_top = . + 0x4000;  /* 16 KB 堆疊 */
}
