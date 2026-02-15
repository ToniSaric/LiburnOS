#ifndef PIC_H_
#define PIC_H_

#include <stdint.h>

void pic_send_eoi(uint8_t irq);
uint16_t pic_get_irr();
uint16_t pic_get_isr();
void pic_irq_set_mask(uint8_t irq_line);
void pic_irq_clear_mask(uint8_t irq_line);
void pic_remap(int offset1, int offset2);
void pic_disable();

#endif
