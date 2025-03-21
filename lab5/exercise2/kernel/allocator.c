#include "header/allocator.h"
#include <stdint.h>
#include "header/uart.h"

#define HEAP_START  0x10000000
#define PAGE_SIZE   4096
#define MAX_ORDER   6
#define FRAME_COUNT (1 << MAX_ORDER)

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
        order++;  // 找到最小可以容納 `size` 的 2^order * PAGE_SIZE
    }
    
    if (order > MAX_ORDER) {
        uart_puts("Memory request too large!\n");
        return 0;
    }

    // **從 `order` 對應的鏈結串列中分配**
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

    // **嘗試合併夥伴 (Buddy Merge)**
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








// **小型記憶體池的區塊大小 (以 16B 為基礎)**
static const uint32_t pool_sizes[] = { 16, 32, 48, 96, 128, 256, 512, 1024 };

// **記憶體池結構**
typedef struct MemoryChunk {
    struct MemoryChunk *next;  // 指向下一個可用區塊
} MemoryChunk;

// **每種大小的記憶體池**
#define POOL_COUNT (sizeof(pool_sizes) / sizeof(pool_sizes[0]))
static MemoryChunk *memory_pools[POOL_COUNT];  // 記錄每個 pool 的可用 chunk

// **對齊至最近的記憶體池大小**
static uint32_t round_up_size(uint32_t size) {
    for (int i = 0; i < POOL_COUNT; i++) {
        if (size <= pool_sizes[i]) return pool_sizes[i];
    }
    return 0;  // 請求超過最大 pool 大小，應該使用 page allocator
}

// **取得記憶體池索引**
static int get_pool_index(uint32_t size) {
    for (int i = 0; i < POOL_COUNT; i++) {
        if (size == pool_sizes[i]) return i;
    }
    return -1;  // 未找到對應的 pool
}

// **動態記憶體分配 (Small Memory Allocator)**
void *dynamic_malloc(uint32_t size) {
    uint32_t aligned_size = round_up_size(size);
    if (aligned_size == 0) {
        // 若請求超過 memory pool 範圍，則直接從夥伴系統分配
        return buddy_malloc(size);
    }

    int pool_index = get_pool_index(aligned_size);
    if (pool_index == -1) {
        uart_puts("Invalid memory pool request!\n");
        return 0;
    }

    // **若記憶體池內有可用的 chunk，直接取出**
    if (memory_pools[pool_index]) {
        MemoryChunk *chunk = memory_pools[pool_index];
        memory_pools[pool_index] = chunk->next;  // 移動可用 chunk 指標
        uart_puts("Allocated chunk from pool: ");
        uart_send_hex((uintptr_t)chunk);
        uart_puts("\n");
        return chunk;
    }

    // **若沒有可用 chunk，則分配一個新的頁框**
    void *page = buddy_malloc(PAGE_SIZE);
    if (!page) {
        uart_puts("Out of memory!\n");
        return 0;
    }

    uart_puts("Allocating new page for pool: ");
    uart_send_hex((uintptr_t)page);
    uart_puts("\n");

    // **將頁框劃分為 chunk，放入記憶體池**
    MemoryChunk *chunk_list = (MemoryChunk *)page;
    int chunk_count = PAGE_SIZE / aligned_size;

    for (int i = 1; i < chunk_count; i++) {
        ((MemoryChunk *)((uintptr_t)page + i * aligned_size))->next = 
            (MemoryChunk *)((uintptr_t)page + (i + 1) * aligned_size);
    }
    ((MemoryChunk *)((uintptr_t)page + (chunk_count - 1) * aligned_size))->next = 0;

    memory_pools[pool_index] = chunk_list->next;  // 記錄剩餘 chunk
    return chunk_list;
}

// **釋放記憶體 (Small Memory Free)**
void dynamic_free(void *ptr, uint32_t size) {
    uint32_t aligned_size = round_up_size(size);
    if (aligned_size == 0) {
        // 若記憶體來自 page allocator，則直接釋放
        buddy_free(ptr);
        return;
    }

    int pool_index = get_pool_index(aligned_size);
    if (pool_index == -1) {
        uart_puts("Invalid memory pool release!\n");
        return;
    }

    // **將 chunk 放回對應的記憶體池**
    MemoryChunk *chunk = (MemoryChunk *)ptr;
    chunk->next = memory_pools[pool_index];
    memory_pools[pool_index] = chunk;

    uart_puts("Freed chunk back to pool: ");
    uart_send_hex((uintptr_t)chunk);
    uart_puts("\n");
}

