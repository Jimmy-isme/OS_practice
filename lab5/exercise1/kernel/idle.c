#include "header/scheduler.h"
#include "header/thread.h"
#include "header/allocator.h"
#include "header/idle.h"

static thread_t *zombie_list = NULL;

// **回收殭屍執行緒**
void kill_zombies() {
    thread_t *prev = NULL, *t = zombie_list;

    while (t) {
        thread_t *next = t->next;

        // **釋放執行緒的堆疊記憶體**
        free_stack(t->stack);
        t->stack = NULL;
        t->state = READY;

        if (prev) {
            prev->next = next;
        } else {
            zombie_list = next;
        }
        t = next;
    }
}

void idle() {
    while (1) {
        kill_zombies();  // **清理殭屍執行緒**
        schedule();
    }
}

