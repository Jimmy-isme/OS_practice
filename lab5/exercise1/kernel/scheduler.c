#include "header/scheduler.h"
#include "header/thread.h"
#include "header/switch.h"
#include <stddef.h>

static thread_t *run_queue = NULL;
static thread_t *current = NULL;

void enqueue_run_queue(thread_t *t) {
    t->next = run_queue;
    run_queue = t;
}

thread_t* pick_next_thread() {
    if (!run_queue) return NULL;
    thread_t *t = run_queue;
    run_queue = run_queue->next;
    return t;
}

void schedule() {
    thread_t *next = pick_next_thread();
    if (next) {
        switch_to(current, next);
        current = next;
    }
}

