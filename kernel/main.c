#include "screen.h"

void kmain()
{
    screen_clear();
    screen_set_cursor(0);

    screen_print("Toni");
    for (int i = 0; i < 5000000; i++){}
    for (int i = 0; i < 5000000; i++){}
    for (int i = 0; i < 5000000; i++){}
    for (int i = 0; i < 5000000; i++){}
    for (int i = 0; i < 5000000; i++){}
    for (;;){}
}