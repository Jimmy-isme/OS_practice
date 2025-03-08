#include "header/allocator.h"

#define HEAP_START 0x10000000  // 記憶體開始的位址 (隨便選一個高位址)

static char *current_alloc = (char *)HEAP_START;  // 目前分配到的位置

void* simple_malloc(size_t size) {
    char *allocated = current_alloc;  // 記錄當前可用地址
    current_alloc += size;  // 移動指針，確保下一次分配時不會覆蓋

    return (void*)allocated;  // 回傳新的記憶體位址
}

