#include "header/allocator.h"
#include "header/uart.h"

#define HEAP_START  0x10000000  // 記憶體起始位址
#define PAGE_SIZE   4096        // 頁框大小 4KB
#define MAX_ORDER   6           // 最高階級 (最大區塊 64KB)
#define FRAME_COUNT (1 << MAX_ORDER)  // 最大可用的頁框數量

typedef struct Frame {
    int order;   // 區塊大小的指數 (2^order * 4KB)
    uint8_t free;  // 0: 已分配, 1: 可用
    struct Frame *next; // 指向鏈結串列的下一個 Frame
} Frame;

static Frame frame_array[FRAME_COUNT];  // Frame 陣列 (代表記憶體狀態)
static Frame *free_list[MAX_ORDER + 1]; // 每種大小的鏈結串列 (free list)

// **初始化夥伴系統**
void init_allocator() {
    for (int i = 0; i <= MAX_ORDER; i++) {
        free_list[i] = 0;  // 初始化鏈結串列
    }
    
    // 將整塊記憶體 (64KB) 設為可用
    frame_array[0].order = MAX_ORDER;
    frame_array[0].free = 1;
    frame_array[0].next = 0;
    
    free_list[MAX_ORDER] = &frame_array[0];  // 加入最大區塊

    uart_puts("Allocator initialized.\n");
}

// **尋找適合大小的區塊**
void *buddy_malloc(uint32_t size) {
    int order = 0;
    while ((1 << order) * PAGE_SIZE < size) {
        order++;  // 找到最小可以容納 size 的 2^order * PAGE_SIZE
    }
    
    if (order > MAX_ORDER) {
        uart_puts("Memory request too large!\n");
        return 0;
    }

    // **從 order 對應的鏈結串列中分配**
    int current_order = order;
    while (current_order <= MAX_ORDER && free_list[current_order] == 0) {
        current_order++;  // 如果該大小沒有可用區塊，搜尋更大的區塊
    }

    if (current_order > MAX_ORDER) {
        uart_puts("Out of memory!\n");
        return 0;
    }

    // **找到更大的區塊，進行拆分 (Split)**
    Frame *block = free_list[current_order];
    free_list[current_order] = block->next;  // 從 free list 移除

    while (current_order > order) {
        current_order--;  // 逐步拆分區塊

        Frame *buddy = &frame_array[block - frame_array + (1 << current_order)];
        buddy->order = current_order;
        buddy->free = 1;
        buddy->next = free_list[current_order];
        free_list[current_order] = buddy;
    }

    block->order = order;
    block->free = 0;

    uart_puts("Allocated memory at index: ");
    uart_send_hex(block - frame_array);
    uart_puts("\n");

    return (void *)(HEAP_START + (block - frame_array) * PAGE_SIZE);
}

// **釋放記憶體**
void buddy_free(void *ptr) {
    int index = ((uintptr_t)ptr - HEAP_START) / PAGE_SIZE;
    Frame *block = &frame_array[index];

    block->free = 1;
    uart_puts("Freeing block at index: ");
    uart_send_hex(index);
    uart_puts("\n");

    // **嘗試合併夥伴**
    while (block->order < MAX_ORDER) {
        int buddy_index = index ^ (1 << block->order); // XOR 計算夥伴索引
        Frame *buddy = &frame_array[buddy_index];

        if (!buddy->free || buddy->order != block->order) {
            break; // 如果夥伴不可合併，停止
        }

        // **合併區塊**
        if (buddy_index < index) {
            block = buddy;
            index = buddy_index;
        }
        block->order++;

        uart_puts("Merging block at index ");
        uart_send_hex(index);
        uart_puts(" with buddy at index ");
        uart_send_hex(buddy_index);
        uart_puts(" into order ");
        uart_send_hex(block->order);
        uart_puts("\n");
    }

    // **將合併後的區塊放回 free list**
    block->next = free_list[block->order];
    free_list[block->order] = block;
}

// **列印記憶體狀態**
void print_memory_state() {
    uart_puts("\nMemory State:\n");
    for (int i = 0; i <= MAX_ORDER; i++) {
        uart_puts("Order ");
        uart_send_hex(i);
        uart_puts(": ");
        Frame *current = free_list[i];
        while (current) {
            uart_send_hex(current - frame_array);
            uart_puts(" -> ");
            current = current->next;
        }
        uart_puts("NULL\n");
    }
}

