#include "process.h"
#include "thread.h"
#include "allocator.h"
#include "scheduler.h"

static process_t processes[MAX_PROCESSES];
static int process_count = 0;

process_t* create_process() {
    if (process_count >= MAX_PROCESSES) return NULL;
    process_t* p = &processes[process_count++];
    p->pid = process_count;
    p->stack = (uint64_t*) allocate_stack();
    return p;
}

void destroy_process(process_t* proc) {
    free_stack(proc->stack);
    proc->stack = NULL;
}

int fork() {
    process_t *parent = current_thread();
    process_t *child = create_process();
    if (!child) return -1;  // **無法建立新行程**

    child->parent_pid = parent->pid;
    return child->pid;
}

int exec(const char *name, char *const argv[]) {
    uart_async_puts("Executing new process...\n");
    // **這裡應該讀取 `name` 指定的執行檔並執行**
    return 0;
}

