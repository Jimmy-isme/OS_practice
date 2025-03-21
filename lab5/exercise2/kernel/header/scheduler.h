#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "thread.h"

void schedule();
void enqueue_run_queue(thread_t *t);
thread_t* pick_next_thread();

#endif

