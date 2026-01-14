#ifndef MEMORY_H_
#define MEMORY_H_

extern void *memcpy(void *dest, const void *src, size_t n)
extern void *memmove(void *dest, const void *src, size_t n)
extern void *memset(void *dest, int val, size_t n)
extern int memcmp(const void *s1, const void *s2, size_t n)

#endif