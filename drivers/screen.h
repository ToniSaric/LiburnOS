#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdint.h>

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
// Attribute byte for colour scheme
#define WHITE_ON_BLACK 0x0f

// Screen device I/O ports
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

#define REG_CURSOR_HIGH 14
#define REG_CURSOR_LOW 15

void screen_init();
void screen_clear();
void screen_set_color(uint8_t fg_color, uint8_t bg_color);
void screen_set_cursor(uint32_t offset);
uint32_t screen_get_cursor();
void screen_scroll(void);
void screen_putc(char c);
void screen_print(const char *str);
#endif