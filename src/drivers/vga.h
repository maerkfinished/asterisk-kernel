#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>
#include "../kernel/libraries/string.h"

#define VGA_TEXT_ADDR 0xB8000
#define VGA_PX_ADDR   0xA0000
#define VGA_WIDTH     80
#define VGA_HEIGHT    25
#define VGA_PX_WIDTH  640
#define VGA_PX_HEIGHT 480
#define PRINT_CLR     0x0F

typedef struct {
    volatile uint16_t *base;
    size_t size;
} buffer;


extern uint16_t cursor_x;
extern uint16_t cursor_y;
extern bool quiet_mode;

void configure_cursor(bool enabled);
void update_cursor(uint16_t x, uint16_t y);
void clear(uint16_t color);
void scroll(int32_t scroll_len, uint16_t color);
void print(string str, uint16_t color);
void printchar(char c, uint16_t color);
void printhex(uint32_t hex, uint16_t color);

void vga_init();


#endif