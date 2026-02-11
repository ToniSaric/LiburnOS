#ifndef STRING_H_
#define STRING_H_

#include <stddef.h>
#include <stdint.h>

/**
 * Convert a signed integer to a string in the given base.
 * Supports bases 2 through 16.
 * buf must be large enough to hold the result (max 34 bytes for base 2 + sign + null).
 * Returns pointer to buf.
 */
char *itoa(int32_t value, char *buf, int base);

/**
 * Convert an unsigned integer to a string in the given base.
 * Supports bases 2 through 16.
 * buf must be large enough to hold the result (max 33 bytes for base 2 + null).
 * Returns pointer to buf.
 */
char *utoa(uint32_t value, char *buf, int base);

#endif
