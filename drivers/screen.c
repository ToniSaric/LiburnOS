/**
 * screen.c
 *
 * VGA Text Mode Screen Driver
 *
 * This file provides basic screen output functionality using the
 * VGA text-mode framebuffer.
 *
 * --------------------------------------------------------------------
 * SCREEN ORGANIZATION
 * --------------------------------------------------------------------
 *
 * The VGA text screen is organized as a grid of:
 *     80 columns × 25 rows
 *
 * Each position on the screen is called a "cell".
 * Each cell occupies **2 bytes** in memory:
 *
 *     Byte 0 : ASCII character to display
 *     Byte 1 : Attribute (foreground + background color)
 *
 * The framebuffer starts at physical memory address:
 *     0xB8000
 *
 * Memory is laid out in **row-major order**, left-to-right, top-to-bottom:
 *     Row 0, Col 0 → first cell
 *     Row 0, Col 1 → second cell
 *     ...
 *     Row 1, Col 0 → first cell of second row
 *
 * The byte offset for a given row and column is calculated as:
 *     offset = 2 * (row * MAX_COLS + col)
 *
 * --------------------------------------------------------------------
 * ATTRIBUTE BYTE FORMAT
 * --------------------------------------------------------------------
 * The attribute byte controls the colors of the character:
 *     Bits 7–4 : Background color
 *     Bits 3–0 : Foreground color
 *
 */

#include "screen.h"
#include "port.h"

static uint8_t screen_attr = WHITE_ON_BLACK;

static uint32_t offset_from_row_col(uint32_t row, uint32_t column);
static uint32_t row_from_offset(uint32_t offset);
static uint32_t col_from_offset(uint32_t offset);

/**
 * @brief Clear the entire VGA text-mode screen.
 *
 * This function fills the VGA text-mode framebuffer with space characters
 * using the default color attribute (WHITE_ON_BLACK).
 *
 * Each screen cell occupies two bytes in memory:
 *   - byte 0: ASCII character
 *   - byte 1: color attribute
 *
 * The screen is cleared by iterating over all rows and columns and writing
 * a space character and attribute byte to each cell.
 *
 * After calling this function, the screen contents are empty but the cursor
 * position is not modified.
 */
void screen_clear()
{
    volatile uint8_t *video_memory = (volatile uint8_t *) VIDEO_ADDRESS;
    for (int i = 0; i < MAX_ROWS * MAX_COLS; i++)
    {
        video_memory[i * 2] = ' ';                      // Character
        video_memory[i * 2 + 1] = screen_attr;          // Attribute
    }
}

void screen_set_color(uint8_t fg_color, uint8_t bg_color)
{
    screen_attr = bg_color << 4 | (fg_color & 0x0F);
}

/**
 * @brief Set the hardware text cursor position.
 *
 * This function updates the VGA hardware cursor to point to a new screen
 * position.
 *
 * The VGA controller stores the cursor position as a cell index (not a
 * byte offset). Therefore, the supplied byte offset is first converted
 * into a cell offset by dividing by two.
 *
 * The cursor position is written as a 16-bit value split across two VGA
 * internal registers:
 *   - Cursor High Register (upper 8 bits)
 *   - Cursor Low Register  (lower 8 bits)
 *
 * @param offset Cursor position expressed as a byte offset in video memory.
 */
void screen_set_cursor(uint32_t offset)
{
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, REG_CURSOR_HIGH);
    port_byte_out(REG_SCREEN_DATA, (uint8_t)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, REG_CURSOR_LOW);
    port_byte_out(REG_SCREEN_DATA, (uint8_t)(offset & 0xff));
}

/**
 * @brief Get the current hardware text cursor position.
 *
 * This function reads the current cursor position from the VGA controller.
 *
 * The VGA hardware reports the cursor position as a cell index. The value
 * is read by querying two internal registers:
 *   - Cursor High Register (upper 8 bits)
 *   - Cursor Low Register  (lower 8 bits)
 *
 * The returned value is converted from a cell index into a byte offset
 * suitable for direct use with VGA text-mode video memory.
 *
 * @return The current cursor position as a byte offset in video memory.
 */
uint32_t screen_get_cursor()
{
    port_byte_out(REG_SCREEN_CTRL, REG_CURSOR_HIGH);
    uint32_t offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, REG_CURSOR_LOW);
    offset += port_byte_in(REG_SCREEN_DATA);

    return offset * 2;
}

void screen_print(const char *str)
{
    if (!str)
    {
        return;
    }

    while(*str)
    {
        screen_putc(*str++);
    }
}

void screen_putc(char c)
{
    volatile uint8_t *video_memory = (volatile uint8_t *) VIDEO_ADDRESS;
    // Get current position
    uint32_t offset = screen_get_cursor();
    if (c == '\n')
    {
        // Newline start in next row
        uint32_t row = row_from_offset(offset);
        offset = offset_from_row_col(row + 1, 0);
    }
    else if(c == '\r')
    {
        // Carriage return: start of current row
        uint32_t row = row_from_offset(offset);
        offset = offset_from_row_col(row, 0);
    }
    else if(c == '\b')
    {
        // Backspace: move back one cell and erase (if not at 0)
        if (offset >= 2)
        {
            offset -= 2;
            video_memory[offset] = ' ';
            video_memory[offset + 1] = screen_attr;
        }
    }
    else
    {
        // Normal character
        video_memory[offset] = c;
        video_memory[offset + 1] = screen_attr;
        offset += 2;
    }

    if (offset >= (MAX_ROWS * MAX_COLS * 2))
    {
        screen_scroll();
        offset -= 2 * MAX_COLS;
    }
    screen_set_cursor(offset);
}

void screen_scroll(void)
{
    volatile uint8_t *video_memory = (volatile uint8_t *) VIDEO_ADDRESS;

    for (uint32_t i = 1; i < MAX_ROWS; i++)
    {
        for (uint32_t j = 0; j < MAX_COLS; j++)
        {
            uint32_t src_offset = offset_from_row_col(i, j);
            uint32_t dest_offset = offset_from_row_col(i - 1, j);
            video_memory[dest_offset] = video_memory[src_offset];
            video_memory[dest_offset + 1] = video_memory[src_offset + 1];
        }
    }

    for (uint32_t j = 0; j < MAX_COLS; j++)
    {
        uint32_t offset = offset_from_row_col(MAX_ROWS - 1, j);
        video_memory[offset] = ' ';
        video_memory[offset + 1] = screen_attr;
    }
}


static uint32_t offset_from_row_col(uint32_t row, uint32_t column)
{
    return 2 * (row * MAX_COLS + column);
}

static uint32_t row_from_offset(uint32_t offset)
{
    return offset / (2 * MAX_COLS);
}

static uint32_t col_from_offset(uint32_t offset)
{
    return (offset / 2) % MAX_COLS;
}