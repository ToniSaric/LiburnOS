#include "kprintf.h"
#include "screen.h"
#include "string.h"

#include <stdint.h>
#include <stdarg.h>

void kprintf(const char *fmt, ...)
{
    if (!fmt) return;

    va_list args;
    va_start(args, fmt);
    while(*fmt)
    {
        if (*fmt == '%')
        {
            char tmp[32]; // temp buffer
            // Skip % and move to next char
            fmt++;
            // Guard if after % is nothing
            if (*fmt == '\0') break;

            if (*fmt == 'd' || *fmt == 'i')
            {
                itoa(va_arg(args, int), tmp, 10);
                screen_print(tmp);
            }
            else if (*fmt == 'c')
            {
                screen_putc((char)va_arg(args, int));
            }
            else if (*fmt == 's')
            {
                const char *str = va_arg(args, const char *);
                if (str)
                {
                    screen_print(str);
                }
                else
                {
                    screen_print("(null)");
                }
            }
            else if (*fmt == 'u')
            {
                utoa(va_arg(args, uint32_t), tmp, 10);
                screen_print(tmp);
            }
            else if (*fmt == 'x' || *fmt == 'X')
            {
                itoa(va_arg(args, int), tmp, 16);
                screen_print(tmp);
            }
            else if (*fmt == 'o')
            {
                utoa(va_arg(args, uint32_t), tmp, 8);
                screen_print(tmp);
            }
            else if (*fmt == 'p')
            {
                screen_print("0x");
                utoa((uint32_t)va_arg(args, void *), tmp, 16);
                screen_print(tmp);
            }
            else if (*fmt == '%')
            {
                screen_putc('%');
            }
        }
        else
        {
            screen_putc(*fmt);
        }
        fmt++;
    }

    va_end(args);
}