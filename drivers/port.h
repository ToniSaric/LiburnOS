#ifndef PORT_H_
#define PORT_H_

#include <stdint.h>

extern uint8_t port_byte_in(uint16_t port);
extern void port_byte_out(uint16_t port, uint8_t data);
extern uint16_t port_word_in(uint16_t port);
extern void port_word_out(uint16_t port, uint16_t data);

#endif