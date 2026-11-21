#include "vga.h"
#include "driver.h"
#include "../kernel/libraries/hwreg.h"

extern const string hex_chars;

driver_t VGA_DRIVER = {
    .name = "VGA Standard TextType Driver",
    .version = "0.5.1",
    .init = vga_init,
};

buffer VGA_BUF = {
    .base = (volatile uint16_t*) VGA_ADDR,
    .size = VGA_WIDTH * VGA_HEIGHT,
};
virtbuffer VIRTUAL_VGA[VGA_WIDTH * VGA_HEIGHT];
vbufdata VIRTUAL_VGA_DATA = {
    .size = VGA_WIDTH * VGA_HEIGHT,
};

uint16_t cursor_x = 0;
uint16_t cursor_y = 0;
bool quiet_mode = false;


static inline uint16_t vgaconvlin(uint16_t x, uint16_t y) {
    return y * VGA_WIDTH + x;
}

bool VIRTW  = true;
bool CSTPOS = false;
bool WBUF_VIRT_ONLY = false;
uint16_t CSTX = 0;
int32_t CSTY  = 0;
static void wbufvga(size_t idx, uint16_t c, uint16_t color) {

    if (color == 0) {
        color = PRINT_CLR;
    }
    if (CSTPOS) {
        idx = CSTY * VGA_WIDTH + CSTX;
    }

    if (idx < VGA_BUF.size && idx < VIRTUAL_VGA_DATA.size) {
        if (!WBUF_VIRT_ONLY) VGA_BUF.base[idx] = color << 8 | (uint8_t) c;
        if (VIRTW) {
            VIRTUAL_VGA[idx].clr = color;
            VIRTUAL_VGA[idx].c   = c;
            VIRTUAL_VGA[idx].x   = idx % VGA_WIDTH;
            VIRTUAL_VGA[idx].y   = idx / VGA_WIDTH;
        }
    }

}
// static uint16_t rbufvga(size_t idx) {
//     if (idx < VGA_BUF.size) return VGA_BUF.base[idx]; else return 0;
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

void kprint_int(int num, uint16_t color) {
    if (quiet_mode) return;

    if (color == 0x00) {
        color = PRINT_CLR;
    }
    char* buf = (char*) num;

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
    size_t i     = 1;

    while (i <= pos && (VIRTUAL_VGA[pos - i].c == '\0')) {
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

static void update_virtvga() {
    bool _copy = WBUF_VIRT_ONLY;
    WBUF_VIRT_ONLY = true;
    for (size_t i = 0; i < VGA_BUF.size; i++) {
        wbufvga(i, (char) (VGA_BUF.base[i] & 0xFF), (VGA_BUF.base[i] >> 8) & 0xFF);
    }
    WBUF_VIRT_ONLY = _copy;
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
        wbufvga(i, ' ', color);
    }

    update_cursor(0, 0);
}

void scroll(int32_t scroll_len) {
    if (scroll_len == 0) return;

    bool _copy_virtw = VIRTW;
    bool _copy_cst   = CSTPOS;
    VIRTW  = false;
    CSTPOS = true;

    int32_t end    = VGA_HEIGHT + scroll_len;
    uint16_t pos   = 0;
    
    while (CSTY < end) {
        if (CSTX >= VGA_WIDTH) {
            CSTX   = 0;
            CSTY   ++;
        }
        if (CSTY > VGA_HEIGHT + 1) {
            CSTY = 0;
        }
        pos = (CSTY + scroll_len) * VGA_WIDTH + CSTX;
        wbufvga(0, VIRTUAL_VGA[pos].c, VIRTUAL_VGA[pos].clr);
        CSTX ++;
    }
    update_virtvga();
    VIRTW = _copy_virtw;
    CSTPOS = _copy_cst;

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
            wbufvga(pos, ' ', color);
        } else {
            backspace_cursor();
        }
    } else if (c == '\t') {
        for (uint8_t i = 0; i < 4; i++) {
            wbufvga(pos + i, ' ', color);
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

    // if (cursor_y >= VGA_HEIGHT) {
    //     scroll(1);
    // }

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