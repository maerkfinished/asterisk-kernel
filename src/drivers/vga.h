#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include "../kernel/libraries/string.h"
#include "../kernel/libraries/math.h"

#define VGA_WIDTH            80
#define VGA_HEIGHT           25
#define STANDARD_PRINT_COLOR 0x0F
#define VGA                  0xB8000

typedef struct {
    char letter;
    uint16_t color;
    struct {
        int x;
        int y;
    } position;
} xystruct[];

extern xystruct vga_screen;
extern int cursor_row;
extern int cursor_col;
extern bool enable_quiet;


void configure_cursor(bool enabled);
void set_cursor(int x, int y);
void clear(uint16_t color);
void scroll(int dir, int times);
void kputchar(char c, uint16_t color);
void kprint_int(int num, uint16_t color);
void print(string msg, uint16_t color );
void kprint_hex(uint32_t hex, uint16_t color);

void vga_init();


#endif