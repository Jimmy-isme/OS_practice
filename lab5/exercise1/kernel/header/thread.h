#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

#define STACK_SIZE  4096
#define MAX_THREADS 16

typedef enum { RUNNING, READY, ZOMBIE } thread_state_t;

typedef struct thread {
    uint64_t *stack;
    uint64_t id;
    thread_state_t state;
    struct thread *next;
} thread_t;

void thread_create(void (*func)());
void thread_exit();
thread_t* current_thread();

void *allocate_stack();
void free_stack(void *stack);

#endif

