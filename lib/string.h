#ifndef STRING_H_
#define STRING_H_

#include <stddef.h>
#include <stdint.h>

char *itoa(int32_t value, char *buf, int base);
char *utoa(uint32_t value, char *buf, int base);

#endif
