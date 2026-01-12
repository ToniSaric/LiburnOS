#include <stdint.h>

/**
 * @brief Read a byte from an I/O port.
 *
 * Assembly details:
 * - `%0` receives the value read from the port (AL register)
 * - `%1` specifies the port number
 *
 * Constraints:
 * - `"=a"(result)` stores the value from AL into `result`
 * - `"Nd"(port)` selects either an immediate 8-bit port constant (if possible)
 *   or the DX register (otherwise)
 *
 * The `"memory"` clobber helps prevent the compiler from reordering memory
 * accesses around this I/O operation.
 *
 * @param port The I/O port number.
 * @return The byte read from the specified port.
 */
uint8_t port_byte_in(uint16_t port)
{
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port) : "memory");
    return result;
}

/**
 * @brief Write a byte to an I/O port.
 *
 * Assembly details:
 * - `%0` is the value written (AL register)
 * - `%1` is the port number
 *
 * Constraints:
 * - `"a"(data)` loads AL with the byte to write
 * - `"Nd"(port)` uses an immediate port if possible, else DX
 *
 * @param port The I/O port number.
 * @param data The byte to write.
 */
void port_byte_out(uint16_t port, uint8_t data)
{
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port) : "memory");
}

/**
 * @brief Read a 16-bit word from an I/O port.
 *
 * Assembly details:
 * - `%0` receives the value read from the port (AX register)
 * - `%1` specifies the port number
 *
 * Constraints:
 * - `"=a"(result)` stores the value from AX into `result`
 * - `"Nd"(port)` selects either an immediate port constant or DX
 *
 * @param port The I/O port number.
 * @return The 16-bit word read from the specified port.
 */
uint16_t port_word_in(uint16_t port)
{
    uint16_t result;
    __asm__ volatile ("inw %1, %0" : "=a"(result) : "Nd"(port) : "memory");
    return result;
}

/**
 * @brief Write a 16-bit word to an I/O port.
 *
 * Assembly details:
 * - `%0` is the value written (AX register)
 * - `%1` is the port number
 *
 * Constraints:
 * - `"a"(data)` loads AX with the word to write
 * - `"Nd"(port)` uses an immediate port if possible, else DX
 *
 * @param port The I/O port number.
 * @param data The 16-bit value to write.
 */
void port_word_out(uint16_t port, uint16_t data)
{
    __asm__ volatile ("outw %0, %1" : : "a"(data), "Nd"(port) : "memory");
}
