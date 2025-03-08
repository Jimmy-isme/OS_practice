#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stddef.h>

int string_compare(const char* i, const char* j);
int string_starts_with(const char *s, const char *prefix);
unsigned long atoi(const char *s, int char_size);
void align(void *size, unsigned int s);
uint32_t align_up(uint32_t size, int alignment);
size_t strlen(const char *s);
void *memcpy(void *dest, const void *src, size_t n);

#endif

