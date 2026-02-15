#include "screen.h"
#include "kprintf.h"
#include "idt.h"     // Add this
#include "pic.h"
#include "keyboard.h"
// ...


void kmain()
{
    screen_clear();
    screen_set_cursor(0);

    int version = 1;
    int revision = 0;
    kprintf("Hello Welcome to LiburnOS revision %d.%d\n", version, revision);

    pic_remap(0x20, 0x28);
    kprintf("PIC remapped.\n");
    idt_init();
    kprintf("IDT Initialized. Interrupts enabled.\n");
    keyboard_init();
    for (;;){}
}
