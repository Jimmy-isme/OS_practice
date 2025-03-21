#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define MAX_PROCESSES  16

typedef struct process {
    uint64_t pid;
    uint64_t parent_pid;
    uint64_t *stack;
    struct process *next;
} process_t;

process_t* create_process();
void destroy_process(process_t* proc);
void schedule_process();
void switch_process(process_t* prev, process_t* next);

#endif

