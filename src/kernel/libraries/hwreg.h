#ifndef HWMGR_H
#define HWMGR_H

#include <stdint.h>

#define REG_CRTC_ADDR    0x3D4
#define REG_CRTC_DATA    0x3D5
#define REG_CUR_START    0x0A
#define REG_CUR_END      0x0B
#define REG_CUR_LOC_HIGH 0x0E
#define REG_CUR_LOC_LOW  0x0F
#define REG_CUR_DISABLE  0x20
#define REG_CUR_ENABLE_T 0x0E
#define REG_CUR_ENABLE_B 0x0F

uint8_t inb(uint16_t port);
void    outb(uint16_t port, uint8_t val);

void hwreg_driver_init();

#endif