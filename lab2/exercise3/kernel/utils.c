#include "header/utils.h"

int string_compare(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int string_starts_with(const char *s, const char *prefix) {
    while (*prefix) {
        if (*s != *prefix) return 0;
        s++;
        prefix++;
    }
    return 1;
}
