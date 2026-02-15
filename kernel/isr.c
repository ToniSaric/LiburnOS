#include <stdint.h>
#include "../drivers/screen.h"
#include "../lib/kprintf.h"
#include "../drivers/pic.h"
#include "../drivers/keyboard.h"

// Exception names for better debugging
static const char* exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"
};

void exception_handler(uint32_t error_code, uint32_t interrupt_num)
{
    // Note: Parameters are reversed on stack (interrupt_num is pushed last)
    // So we declare them reversed: error_code, interrupt_num
    if (interrupt_num == 33)
    {
        keyboard_handler();
        pic_send_eoi(1);  // Send EOI for IRQ1
        return;
    }
    if (interrupt_num >= 32 && interrupt_num <= 47)
    {
        uint8_t irq = interrupt_num - 32;
        // TODO: dispatch to device handlers
        pic_send_eoi(irq);
        return;
    }
    kprintf("\n\n=== KERNEL PANIC ===\n");
    kprintf("Exception: %s (#%d)\n", 
            exception_messages[interrupt_num], 
            interrupt_num);
    kprintf("Error Code: 0x%x\n", error_code);
    
    // Additional info for specific exceptions
    if (interrupt_num == 14)
    {
        // Page Fault
        uint32_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
        kprintf("Faulting Address (CR2): 0x%x\n", cr2);
    }
    
    kprintf("\nSystem Halted.\n");
    
    __asm__ volatile ("cli; hlt");
    while(1);  // Prevent compiler warnings
}