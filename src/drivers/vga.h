#ifndef VGA_H
#define VGA_H

#include "../kernel/libraries/string.h"

#define VGA_ADDR   0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

typedef struct {
    uint8_t *base;
    uint32_t size;
} buffer;


void print(string str, uint16_t color);
void printchar(char c, uint16_t color);


#endif