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
#include "memory.h"

static uint8_t screen_attr = WHITE_ON_BLACK;
static uint32_t cursor_cell;;

static uint32_t cell_from_row_col(uint32_t row, uint32_t column);
static uint32_t row_from_cell(uint32_t cell);
static uint32_t col_from_cell(uint32_t cell);


void screen_init()
{
    screen_attr = WHITE_ON_BLACK;
    screen_clear();
    cursor_cell = 0;
    screen_set_cursor(0);
}

/**
 * @brief Clear the entire VGA text-mode screen.
 *
 * Fills the VGA text-mode framebuffer with space characters using the
 * current attribute byte (`screen_attr`). Each cell is written as a 16-bit
 * value: high byte = attribute, low byte = ASCII character.
 *
 * @note This function does not change the hardware cursor position.
 */
void screen_clear(void)
{
    volatile uint16_t *video_memory = (volatile uint16_t *) VIDEO_ADDRESS;
    for (uint32_t i = 0; i < MAX_ROWS * MAX_COLS; i++)
    {
        video_memory[i] = screen_attr << 8 | ' ';
    }
}

/**
 * @brief Set the current text attribute (foreground/background color).
 *
 * Updates the global attribute used by subsequent screen output.
 *
 * @param fg_color Foreground color (low 4 bits used).
 * @param bg_color Background color (low 4 bits used; stored in high nibble).
 */
void screen_set_color(uint8_t fg_color, uint8_t bg_color)
{
    screen_attr = bg_color << 4 | (fg_color & 0x0F);
}

/**
 * @brief Set the VGA hardware cursor position (in cells).
 *
 * Writes the cursor position to the VGA controller via I/O ports. The VGA
 * hardware stores the cursor as a cell index (0..MAX_ROWS*MAX_COLS-1),
 * split across two internal registers (high and low byte).
 *
 * @param cell Cursor position as a cell index.
 */
void screen_set_cursor(uint32_t cell)
{
    port_byte_out(REG_SCREEN_CTRL, REG_CURSOR_HIGH);
    port_byte_out(REG_SCREEN_DATA, (uint8_t)(cell >> 8));
    port_byte_out(REG_SCREEN_CTRL, REG_CURSOR_LOW);
    port_byte_out(REG_SCREEN_DATA, (uint8_t)(cell & 0xff));
}

/**
 * @brief Get the current VGA hardware cursor position (in cells).
 *
 * Reads the cursor position from the VGA controller via I/O ports. The VGA
 * reports the cursor as a cell index split across two registers (high and
 * low byte).
 *
 * @return Current cursor position as a cell index.
 */
uint32_t screen_get_cursor(void)
{
    port_byte_out(REG_SCREEN_CTRL, REG_CURSOR_HIGH);
    uint32_t cell = (uint32_t)port_byte_in(REG_SCREEN_DATA) << 8;

    port_byte_out(REG_SCREEN_CTRL, REG_CURSOR_LOW);
    cell |= (uint32_t)port_byte_in(REG_SCREEN_DATA);

    return cell;
}

/**
 * @brief Print a NUL-terminated string to the screen at the current cursor.
 *
 * Outputs characters one-by-one using `screen_putc()`.
 *
 * @param str Pointer to a NUL-terminated string. If NULL, the function
 *            returns without printing anything.
 */
void screen_print(const char *str)
{
    if (!str)
    {
        return;
    }

    while (*str)
    {
        screen_putc(*str++);
    }
}

/**
 * @brief Output a single character to the screen.
 *
 * Writes a character to the VGA text-mode framebuffer at the current cursor
 * position, then advances the cursor. Handles basic control characters:
 * - '\\n' (newline): move cursor to start of next row
 * - '\\r' (carriage return): move cursor to start of current row
 * - '\\b' (backspace): move cursor back one cell and erase it (if possible)
 *
 * If the cursor moves beyond the last screen row, the screen is scrolled
 * up by one line and the cursor is placed on the last row.
 *
 * @param c Character to output.
 *
 * @note This implementation reads and updates the hardware cursor on each
 *       call (via `screen_get_cursor()` / `screen_set_cursor()`).
 */
void screen_putc(char c)
{
    volatile uint16_t *video_memory = (volatile uint16_t *)VIDEO_ADDRESS;

    if (c == '\n')
    {
        uint32_t row = row_from_cell(cursor_cell);
        cursor_cell = cell_from_row_col(row + 1, 0);
    }
    else if (c == '\r')
    {
        uint32_t row = row_from_cell(cursor_cell);
        cursor_cell = cell_from_row_col(row, 0);
    }
    else if (c == '\b')
    {
        if (cursor_cell > 0)
        {
            cursor_cell--;
            video_memory[cursor_cell] = ((uint16_t)screen_attr << 8) | (uint8_t)' ';
        }
    }
    else if (c == '\t')
    {
        /* Advance cursor to the next tab stop (every TAB_WIDTH columns).
         * Fill skipped cells with spaces so they overwrite old content. */
        uint32_t col = col_from_cell(cursor_cell);
        uint32_t next_tab = (col + TAB_WIDTH) & ~(TAB_WIDTH - 1);

        if (next_tab > MAX_COLS)
        {
            next_tab = MAX_COLS;
        }

        while (col < next_tab)
        {
            video_memory[cursor_cell] = ((uint16_t)screen_attr << 8) | (uint8_t)' ';
            cursor_cell++;
            col++;
        }
    }
    else
    {
        video_memory[cursor_cell] = ((uint16_t)screen_attr << 8) | (uint8_t)c;
        cursor_cell++;
    }

    if (cursor_cell >= (MAX_ROWS * MAX_COLS))
    {
        screen_scroll();
        cursor_cell -= MAX_COLS;
    }

    screen_set_cursor(cursor_cell);
}


/**
 * @brief Scroll the screen up by one text row.
 *
 * Moves rows 1..MAX_ROWS-1 up to rows 0..MAX_ROWS-2, then clears the last
 * row by filling it with space characters using the current attribute.
 *
 * @note This function does not directly update the hardware cursor; callers
 *       should adjust the cursor as needed.
 */
void screen_scroll(void)
{
    volatile uint16_t *video_memory = (volatile uint16_t *) VIDEO_ADDRESS;


    memmove((void *) video_memory,
            (void *) (video_memory + MAX_COLS),
            (MAX_ROWS - 1) * MAX_COLS * sizeof(uint16_t));

    memset16((void *) (video_memory + ((MAX_ROWS - 1) * MAX_COLS)),
             (uint16_t) ((screen_attr << 8) | ' '),
             MAX_COLS);
}

/**
 * @brief Convert a (row, column) position to a linear cell index.
 *
 * The VGA text framebuffer is laid out in row-major order:
 * `cell = row * MAX_COLS + column`.
 *
 * @param row    Screen row (0..MAX_ROWS-1).
 * @param column Screen column (0..MAX_COLS-1).
 *
 * @return Linear cell index corresponding to (row, column).
 */
static uint32_t cell_from_row_col(uint32_t row, uint32_t column)
{
    return row * MAX_COLS + column;
}

/**
 * @brief Get the row number corresponding to a linear cell index.
 *
 * @param cell Linear cell index.
 *
 * @return Screen row containing the cell.
 */
static uint32_t row_from_cell(uint32_t cell)
{
    return cell / MAX_COLS;
}

/**
 * @brief Get the column number corresponding to a linear cell index.
 *
 * @param cell Linear cell index.
 *
 * @return Screen column containing the cell.
 */
static uint32_t col_from_cell(uint32_t cell)
{
    return cell % MAX_COLS;
}