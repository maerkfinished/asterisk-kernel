#include "vga.h"
#include "driver.h"
#include "../kernel/libraries/hwreg.h"

driver_t VGA_DRIVER = {
    .name = "VGA Standard Driver",
    .version = "0.5.1",
    .init = vga_init,
};

buffer VGA_BUF = {
    .base = VGA_ADDR,
    .size = 
}


static void wbufchar(char c, buffer buf) {

}

void print(string str, uint16_t color) {

}

void printchar(char c, uint16_t color) {
    
}


void vga_init() {

}