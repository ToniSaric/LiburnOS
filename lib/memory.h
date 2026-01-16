#ifndef MEMORY_H_
#define MEMORY_H_

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *dest, int val, size_t n);
void *memset16(void *dest, uint16_t val, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif