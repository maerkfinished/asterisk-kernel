#include "../../drivers/driver.h"
#include "hwreg.h"

void_driver_t HWREG_DRIVER = {
    .name    = "Hardware Register Map & Utilities Driver",
    .version = "0.1",
    .init    = hwreg_driver_init
};


uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}


void hwreg_driver_init() {
    
}