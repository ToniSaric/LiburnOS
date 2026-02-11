#include "string.h"

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
