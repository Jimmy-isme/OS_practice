#include "header/thread.h"
#include "header/scheduler.h"
#include "header/allocator.h"

#define STACK_SIZE  4096

static thread_t threads[MAX_THREADS];
static int thread_count = 0;
static thread_t *current = NULL;  // **追蹤目前執行的執行緒**

void *allocate_stack() {
    return buddy_malloc(STACK_SIZE);  // **分配 4KB 堆疊**
}

void free_stack(void *stack) {
    buddy_free(stack);  // **釋放記憶體**
}

thread_t* current_thread() {
    return current;  // **回傳當前執行緒**
}

void thread_create(void (*func)()) {
    if (thread_count >= MAX_THREADS) return;

    thread_t *t = &threads[thread_count++];
    t->stack = (uint64_t *) allocate_stack();  // **分配堆疊**
    t->state = READY;
    t->id = thread_count;

    // **設定執行緒的初始堆疊內容**
    t->stack[STACK_SIZE / sizeof(uint64_t) - 2] = (uint64_t) func;

    enqueue_run_queue(t);  // **加入執行佇列**
}

void thread_exit() {
    current->state = ZOMBIE;  // **標記執行緒為殭屍**
    schedule();  // **讓出 CPU，讓排程器決定下一個執行緒**
}

