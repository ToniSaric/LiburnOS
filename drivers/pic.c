#include "pic.h"
#include "port.h"

#define PIC1                0x20        /* IO base address for master PIC */
#define PIC2                0xA0        /* IO base address for slave PIC */
#define PIC1_COMMAND        PIC1
#define PIC1_DATA           (PIC1+1)
#define PIC2_COMMAND        PIC2
#define PIC2_DATA           (PIC2+1)
#define PIC_EOI             0x20        /* End-of-interrupt command code */
#define PIC_READ_IRR        0x0a        /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR        0x0b        /* OCW3 irq service next CMD read */

#define ICW1_ICW4           0x01        /* Indicates that ICW4 will be present */
#define ICW1_SINGLE         0x02        /* Single (cascade) mode */
#define ICW1_INTERVAL4      0x04        /* Call address interval 4 (8) */
#define ICW1_LEVEL          0x08        /* Level triggered (edge) mode */
#define ICW1_INIT           0x10        /* Initialization - required! */

#define ICW4_8086           0x01        /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO           0x02        /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE      0x08        /* Buffered mode/slave */
#define ICW4_BUF_MASTER     0x0C        /* Buffered mode/master */
#define ICW4_SFNM           0x10        /* Special fully nested (not) */

#define CASCADE_IRQ 2

static inline void io_wait(void)
{
    port_byte_out(0x80, 0);
}

static uint16_t pic_get_irq_reg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    port_byte_out(PIC1_COMMAND, ocw3);
    port_byte_out(PIC2_COMMAND, ocw3);
    return (port_byte_in(PIC2_COMMAND) << 8) | port_byte_in(PIC1_COMMAND);
}


void pic_send_eoi(uint8_t irq)
{
    // If the IRQ number is 8 or higher, it means the interrupt came from the slave PIC.
    // then we need to send an EOI to the slave PIC first, followed by the master PIC.
    if (irq >= 8)
    {
        port_byte_out(PIC2_COMMAND, PIC_EOI);
    }
    port_byte_out(PIC1_COMMAND, PIC_EOI);
}

uint16_t pic_get_irr()
{
    return pic_get_irq_reg(PIC_READ_IRR);
}

uint16_t pic_get_isr()
{
    return pic_get_irq_reg(PIC_READ_ISR);
}

void pic_irq_set_mask(uint8_t irq_line)
{
    uint16_t port;
    uint8_t value;

    if (irq_line < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = port_byte_in(port) | (1 << irq_line);
    port_byte_out(port, value);
}

void pic_irq_clear_mask(uint8_t irq_line)
{
    uint16_t port;
    uint8_t value;

    if (irq_line < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = port_byte_in(port) & ~(1 << irq_line);
    port_byte_out(port, value);
}

void pic_remap(int offset1, int offset2)
{
    // Start the initialization sequence (in cascade mode)
    port_byte_out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    port_byte_out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    // ICW2: Master pic vector offset
    port_byte_out(PIC1_DATA, offset1);
    // ICW2: Slave pic vector offset
    port_byte_out(PIC2_DATA, offset2);

    // ICW3: Tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    port_byte_out(PIC1_DATA, 1 << CASCADE_IRQ);
    // ICW3: Tell Slave PIC its cascade identity (0000 0010)
    port_byte_out(PIC2_DATA, CASCADE_IRQ);

    // ICW4: Set both PICs to 8086 mode
    port_byte_out(PIC1_DATA, ICW4_8086);
    port_byte_out(PIC2_DATA, ICW4_8086);

    // Clear the interrupt masks (enable all IRQs)
    port_byte_out(PIC1_DATA, 0x0);
    port_byte_out(PIC2_DATA, 0x0);
}

void pic_disable()
{
    // Mask all interrupts on both PICs
    port_byte_out(PIC1_DATA, 0xFF);
    port_byte_out(PIC2_DATA, 0xFF);
}