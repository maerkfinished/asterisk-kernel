#include "vga.h"
#include "driver.h"
#include "../kernel/libraries/hwreg.h"
#include "../kernel/libraries/stdmem.h"
#include "../kernel/libraries/math.h"

extern const string hex_chars;

void_driver_t VGA_DRIVER = {
    .name = "VGA Standard Driver",
    .version = "0.6",
    .init = vga_init,
};

buffer VGATXT_BUF = {
    .base = (volatile uint16_t*) VGA_TEXT_ADDR,
    .size = VGA_WIDTH * VGA_HEIGHT,
};
buffer VGAPX_BUF = {
    .base = (volatile uint16_t*) VGA_PX_ADDR,
    .size = VGA_PX_WIDTH * VGA_PX_HEIGHT,
};


uint16_t cursor_x = 0;
uint16_t cursor_y = 0;
bool quiet_mode = false;


static inline uint16_t vgaconvlin(uint16_t x, uint16_t y) {
    return y * VGA_WIDTH + x;
}

bool CSTPOS = false;
uint16_t CSTX = 0;
int32_t CSTY  = 0;
size_t CSTPOS32 = 0;
static void wbufvga(size_t idx, uint16_t c, uint16_t color) {

    if (color == 0) {
        color = PRINT_CLR;
    }
    if (CSTPOS) {
        idx = vgaconvlin(CSTX, CSTY);
    }

    if (idx < VGATXT_BUF.size) {
        if (CSTPOS && c == '\n') {
            CSTX = 0;
            CSTY ++;
            CSTPOS32 = vgaconvlin(CSTX, CSTY);
        } else VGATXT_BUF.base[idx] = color << 8 | (uint8_t) c;
    }

}
// static uint16_t rbufvga(size_t idx) {
//     if (idx < VGATXT_BUF.size) return VGATXT_BUF.base[idx]; else return 0;
// }


void configure_cursor(bool enabled) {

    outb(REG_CRTC_ADDR, REG_CUR_START); 
    if (!enabled) {
        outb(REG_CRTC_DATA, REG_CUR_DISABLE);
        return;
    }

    outb(REG_CRTC_DATA, REG_CUR_ENABLE_T);
    outb(REG_CRTC_ADDR, REG_CUR_END);
    outb(REG_CRTC_DATA, REG_CUR_ENABLE_B);
}

void printint(int num, uint16_t color) {
    if (quiet_mode) return;

    if (color == 0x00) {
        color = PRINT_CLR;
    }
    string buf = (string) num;

    if (num == 0) {
        printchar('0', color);
        return;
    } else if (num < 0) {
        printchar('-', color);
        num = -num;
    }

    int i = 0;
    while (num > 0) {
        int n = num % 10;
        buf[i] = '0' + n;
        i++;
        num /= 10;
    }
    while (i > 0) {
        i--;
        printchar(buf[i], color);
    }

}

static void backspace_cursor() {

    cursor_x = VGA_WIDTH;
    cursor_y --;

    uint16_t pos = vgaconvlin(cursor_x, cursor_y);
    uint16_t end = VGA_WIDTH;
    size_t i     = 1;

    while (i < end && ((char) (VGATXT_BUF.base[pos - i] & 0xFF) == '\0' || (char) (VGATXT_BUF.base[pos - i] & 0xFF) == ' ')) {
        i++;
    }
    i--;

    if (i + 1 <= pos) {
        pos -= i;
        if (pos % VGA_WIDTH == 0) {
            pos--;
        }
        
        cursor_x = pos % VGA_WIDTH;
        cursor_y = pos / VGA_WIDTH;
        if (cursor_x == VGA_WIDTH - 1) {
            wbufvga(pos, ' ', 0);
        }

        update_cursor(cursor_x, cursor_y);
    } else {
        update_cursor(0, 0);
    }
}


void update_cursor(uint16_t x, uint16_t y) {
    if (x >= VGA_WIDTH) x = VGA_WIDTH - 1;
    if (y >= VGA_HEIGHT) y = VGA_HEIGHT - 1;

    cursor_x = x;
    cursor_y = y;

    uint16_t pos = vgaconvlin(x, y);

    outb(REG_CRTC_ADDR, 0x0F);
    outb(REG_CRTC_DATA, pos & 0xFF);

    outb(REG_CRTC_ADDR, 0x0E);
    outb(REG_CRTC_DATA, (pos >> 8) & 0xFF);
}

void clear(uint16_t color) {

    if (color == 0) {
        color = PRINT_CLR;
    }

    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        wbufvga(i, '\0', color);
    }

    update_cursor(0, 0);
}

void scroll(int32_t scroll_len, uint16_t color) {
    if (scroll_len == 0) return;
    if (color == 0) {
        color = PRINT_CLR;
    }

    int32_t shift = scroll_len * VGA_WIDTH;
    size_t keep  = VGATXT_BUF.size - shift;

    memmove(VGATXT_BUF.base, VGATXT_BUF.base + shift, keep * sizeof(VGATXT_BUF.size));

    if (scroll_len < 0) {
        for (int32_t i = abs(shift); i > 0; i--) {
            wbufvga(i - 1, ' ', color);
        }
    }

}

void print(string str, uint16_t color) {
    if (quiet_mode) return;

    if (color == 0) {
        color = PRINT_CLR;
    }

    for (size_t i = 0; str[i] != '\0'; i++) {
        printchar(str[i], color);
    }
}

void printchar(char c, uint16_t color) {
    if (quiet_mode) return;

    if (color == 0) {
        color = PRINT_CLR;
    }

    uint16_t pos = vgaconvlin(cursor_x, cursor_y);
    if (c == '\n') {
        cursor_x = 0;
        cursor_y ++;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x --;
            pos = vgaconvlin(cursor_x, cursor_y);
            wbufvga(pos, '\0', color);
        } else {
            backspace_cursor();
        }
    } else if (c == '\t') {
        for (uint8_t i = 0; i < 4; i++) {
            wbufvga(pos + i, '\0', color);
            cursor_x ++;
        }
    } else if (c == '\1') {
        if (cursor_x > 0) {
            cursor_x --;
        } else {
            backspace_cursor();
        }
    } else if (c == '\2') {
        if (cursor_y > 0) {
            cursor_y --;
        }
    } else if (c == '\3') {
        if (cursor_y < VGA_HEIGHT - 1) {
            cursor_y ++;
        }
    }
    else if (c == '\4') {
        if (cursor_x < VGA_WIDTH - 1) {
            cursor_x ++;
        } else {
            cursor_x = 0;
            cursor_y ++;
        }
    }
    else {
        wbufvga(pos, c, color);
        cursor_x ++;

        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y ++;
        }
    }

    if (cursor_y >= VGA_HEIGHT) {
        scroll(1, 0);
    }

    update_cursor(cursor_x, cursor_y);

}

void printhex(uint32_t hex, uint16_t color) {
    if (quiet_mode) return;

    string buf = "0x00000000";

    for (int i = 9; i >= 2; i--) {
        buf[i] = hex_chars[hex & 0xF];
        hex >>= 4;
    }

    print(buf, color);

}


void vga_init() {

}