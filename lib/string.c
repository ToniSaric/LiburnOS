/**
 * string.c
 *
 * String Conversion Utilities
 *
 * This file provides integer-to-string conversion functions for use
 * in a freestanding (no libc) environment.
 *
 * --------------------------------------------------------------------
 * CONVERSION APPROACH
 * --------------------------------------------------------------------
 *
 * Both functions extract digits by repeatedly dividing the value by
 * the requested base, collecting remainders from least-significant to
 * most-significant. The digits are then reversed into the caller's
 * buffer.
 *
 * Supported bases range from 2 (binary) through 16 (hexadecimal).
 * Hexadecimal digits above 9 use lowercase letters (a–f).
 *
 * --------------------------------------------------------------------
 * SIGNED vs UNSIGNED
 * --------------------------------------------------------------------
 *
 * itoa() applies a negative sign only for base 10. For all other
 * bases the value is treated as its unsigned (two's complement) bit
 * pattern. This matches the kernel convention where hexadecimal and
 * binary representations show raw machine values.
 *
 * INT32_MIN (−2147483648) is handled safely by casting through
 * int64_t before negation, avoiding undefined behaviour from negating
 * the most-negative value in a 32-bit signed type.
 *
 */

#include "string.h"

/**
 * @brief Convert an unsigned 32-bit integer to a string in the given base.
 *
 * Extracts digits by repeated division, building the result in reverse
 * order in a temporary buffer, then copies the digits into @p buf in
 * the correct (most-significant first) order.
 *
 * @param value  The unsigned integer to convert.
 * @param buf    Destination buffer. Must be large enough to hold the
 *               result (max 33 bytes for base 2, including NUL).
 * @param base   Numeric base for the conversion (2–16).
 *
 * @return Pointer to @p buf.
 *
 * @note If @p base is out of range, @p buf is set to an empty string.
 */
char *utoa(uint32_t value, char *buf, int base)
{
    static const char digits[] = "0123456789abcdef";

    if (base < 2 || base > 16)
    {
        buf[0] = '\0';
        return buf;
    }

    /* Build the string in reverse */
    char tmp[33];
    int i = 0;

    if (value == 0)
    {
        tmp[i++] = '0';
    }
    else
    {
        while (value > 0)
        {
            tmp[i++] = digits[value % base];
            value /= base;
        }
    }

    /* Copy reversed digits into buf */
    int j = 0;
    while (i > 0)
    {
        buf[j++] = tmp[--i];
    }
    buf[j] = '\0';

    return buf;
}

/**
 * @brief Convert a signed 32-bit integer to a string in the given base.
 *
 * For base 10, negative values are prefixed with '-' and the absolute
 * value is converted. For all other bases, the value is reinterpreted
 * as unsigned (raw two's complement bit pattern) and passed to utoa().
 *
 * @param value  The signed integer to convert.
 * @param buf    Destination buffer. Must be large enough to hold the
 *               result (max 34 bytes for base 2 + sign + NUL).
 * @param base   Numeric base for the conversion (2–16).
 *
 * @return Pointer to @p buf.
 *
 * @note If @p base is out of range, @p buf is set to an empty string.
 * @note INT32_MIN is handled safely via an int64_t cast before negation.
 */
char *itoa(int32_t value, char *buf, int base)
{
    if (base < 2 || base > 16)
    {
        buf[0] = '\0';
        return buf;
    }

    /* Only apply negative sign for base 10 */
    if (value < 0 && base == 10)
    {
        buf[0] = '-';
        /* Handle INT32_MIN (-2147483648) safely: casting to unsigned gives
         * the correct absolute value due to two's complement wraparound */
        utoa((uint32_t)(-(int64_t)value), buf + 1, base);
        return buf;
    }

    /* For non-decimal bases, treat the value as unsigned (raw bit pattern) */
    utoa((uint32_t)value, buf, base);
    return buf;
}
