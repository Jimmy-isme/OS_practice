#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <stddef.h>
#include <stdint.h>

void init_allocator();
void *buddy_malloc(uint32_t size);
void buddy_free(void *ptr);
void *dynamic_malloc(uint32_t size);
void dynamic_free(void *ptr, uint32_t size);
void print_memory_state();

#endif

