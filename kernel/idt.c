#include "idt.h"

#include <stdbool.h>

__attribute__((aligned(0x10))) 
static idt_entry_t idt[256];    // Create an array of IDT entries; aligned for performance
static idtr_t idtr;             // Create an IDTR structure to hold the base and limit of the IDT
static bool vectors[IDT_MAX_DESCRIPTORS]; // Create an array to track which vectors have been set; this can be useful for debugging or future features

extern void* isr_stub_table[]; // Declare an external array of ISR stubs defined in assembly; this will hold the addresses of the ISR stubs

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags)
{
    idt_entry_t *descriptor = &idt[vector];                             // Get the address of the IDT entry for the given vector
    descriptor->isr_low = (uint16_t)((uint32_t)isr & 0xFFFF);           // Set the lower 16 bits of the ISR address
    descriptor->isr_high = (uint16_t)(((uint32_t)isr >> 16) & 0xFFFF);  // Set the higher 16 bits of the ISR address
    descriptor->kernel_cs = 0x08;                                       // Kernel code segment selector (assuming it's the second entry in the GDT)
    descriptor->reserved = 0;                                           // Reserved field must be zero
    descriptor->attributes = flags;                                     // Set the attributes (type and flags)
}

void idt_init()
{
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;
    for (uint16_t vector = 0; vector < 48; vector++)
    {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E); // Set the descriptor for each of the first 32 vectors (CPU exceptions) with appropriate flags
        vectors[vector] = true; // Mark this vector as set
    }

    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag
}