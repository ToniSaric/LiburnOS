#include "keyboard.h"

#include "port.h"
#include "pic.h"
#include "screen.h"
#include "../lib/kprintf.h"

#include <stdint.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Scan Code Set 1 to ASCII lookup table (lowercase only)
// Index = scancode, Value = ASCII character (0 = unmapped)
static const char scancode_to_ascii[128] =
{
     0,    27,   '1',  '2',  '3',  '4',  '5',  '6',   // 0x00: [None] [Esc]  1  2  3  4  5  6
     '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',  // 0x08: 7  8  9  0  -  =  [Backspace]  [Tab]
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',   // 0x10: q  w  e  r  t  y  u  i
     'o',  'p',  '[',  ']',  '\n', 0,    'a',  's',   // 0x18: o  p  [  ]  [Enter]  [LCtrl]  a  s
     'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',   // 0x20: d  f  g  h  j  k  l  ;
     '\'', '`',  0,    '\\', 'z',  'x',  'c',  'v',   // 0x28: '  `  [LShift]  \  z  x  c  v
     'b',  'n',  'm',  ',',  '.',  '/',  0,    '*',   // 0x30: b  n  m  ,  .  /  [RShift]  [KP *]
     0,    ' ',  0,    0,    0,    0,    0,    0,      // 0x38: [LAlt]  [Space]  [Caps]  [F1]  [F2]  [F3]  [F4]  [F5]
     0,    0,    0,    0,    0,    0,    0,    '7',    // 0x40: [F6]  [F7]  [F8]  [F9]  [F10]  [Num]  [Scroll]  [KP 7]
     '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',  // 0x48: [KP 8]  [KP 9]  [KP -]  [KP 4]  [KP 5]  [KP 6]  [KP +]  [KP 1]
     '2',  '3',  '0',  '.',  0,    0,    0,    0,     // 0x50: [KP 2]  [KP 3]  [KP 0]  [KP .]  ...  ...  ...  [F11]
     0,    0,    0,    0,    0,    0,    0,    0,      // 0x58: [F12]  ...
     0,    0,    0,    0,    0,    0,    0,    0,      // 0x60:
     0,    0,    0,    0,    0,    0,    0,    0,      // 0x68:
     0,    0,    0,    0,    0,    0,    0,    0,      // 0x70:
     0,    0,    0,    0,    0,    0,    0,    0,      // 0x78:
};

void keyboard_init()
{
    // Enable the keyboard IRQ (IRQ1)
    pic_irq_clear_mask(1);
}

void keyboard_handler()
{
    uint8_t scancode = port_byte_in(0x60);

    // Skip the 0xE0 extended prefix for now
    if (scancode == 0xE0)
        return;

    // Skip key releases (bit 7 set)
    if (scancode & 0x80)
        return;

    // Only handle key presses - look up in scancode table
    char c = scancode_to_ascii[scancode];
    if (c)
        screen_putc(c);
}