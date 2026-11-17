// VGA Driver made by Maliks00 (maerkfinished)
// Version 0.3 ( Asterisk versioning )
//
// You can use this driver, but it is specifically made for the Asterisk kernel 0.3 so you will probably need to mod some things for it to work YOUR way.
// No liscense for now, this is currently private

#include "driver.h"
#include "vga.h"
#include "../kernel/libraries/hwreg.h"

driver_t vga = {
    .name    = "VGA Text Driver",
    .version = "0.5 ALPHA",
    .init    = vga_init,
};

xystruct vga_screen = {};

int cursor_col         = 0;
int cursor_row         = 0;
bool enable_quiet      = false;
const string hex_chars = "0123456789ABCDEF";

static void hw_update_cursor() {
    uint16_t pos = 0;

    outb(REG_CRTC_ADDR, REG_CUR_LOC_HIGH);
    pos |= ((uint16_t) inb(REG_CRTC_DATA)) << 8;

    outb(REG_CRTC_ADDR, REG_CUR_LOC_LOW);
    pos |= inb(REG_CRTC_DATA);

    cursor_col = pos % VGA_WIDTH;
    cursor_row = pos / VGA_WIDTH;
}


void configure_cursor(bool enabled) {
    outb(REG_CRTC_ADDR, REG_CUR_START); 
    if (!enabled) { outb(REG_CRTC_DATA, REG_CUR_DISABLE); }
    else {
        outb(REG_CRTC_DATA, REG_CUR_ENABLE_T);
        
        outb(REG_CRTC_ADDR, REG_CUR_END);
        outb(REG_CRTC_DATA, REG_CUR_ENABLE_B);
    }
}

static void set_to_last_free_spot() {
    uint16_t linear_pos = (uint16_t)(cursor_row * VGA_WIDTH + cursor_col);
    int i = 1;
    while ((vga_screen[linear_pos - i].letter == ' ' || !vga_screen[linear_pos - i].letter) && i <= linear_pos) {
        i++;
    }
    if (i <= linear_pos) {
        linear_pos -= i;
        cursor_col = linear_pos % VGA_WIDTH;
        cursor_row = linear_pos / VGA_WIDTH;
        set_cursor(cursor_col, cursor_row);
    }
    
}

bool KPUTCHAR_CUSTOM_POS = false;
bool ACK_POS             = false;
int KPUTCHAR_POS_X       = 0;
int KPUTCHAR_POS_Y       = 0;

void set_cursor(int x, int y) {
    if (x < 0) x = 0;
    if (x >= VGA_WIDTH) x = VGA_WIDTH - 1;
    if (y < 0) y = 0;
    if (y >= VGA_HEIGHT) y = VGA_HEIGHT - 1;

    if (!KPUTCHAR_CUSTOM_POS) {
        cursor_col = x;
        cursor_row = y;
    } else {
        KPUTCHAR_POS_X = x;
        KPUTCHAR_POS_Y = y;
    }

    // Offset or linear position, we need this format for the CRTC port inputs.
    uint16_t pos;
    if (!KPUTCHAR_CUSTOM_POS) {
        pos = (uint16_t)(cursor_row * VGA_WIDTH + cursor_col);
    } else {
        pos = (uint16_t)(KPUTCHAR_POS_Y * VGA_WIDTH + KPUTCHAR_POS_X);
    }

    outb(REG_CRTC_ADDR, 0x0F);
    outb(REG_CRTC_DATA, (uint8_t)(pos & 0xFF));

    outb(REG_CRTC_ADDR, 0x0E);
    outb(REG_CRTC_DATA, (uint8_t)((pos >> 8) & 0xFF));
}

bool SECRET_KEEP_VGA_SCREEN_AFTER_CLEAR = false;
void clear(uint16_t color) {
    volatile uint16_t* vga = (volatile uint16_t*) VGA;

    if (color == 0x00) {
        color = STANDARD_PRINT_COLOR;
    }

    uint16_t blank = ((uint16_t) color << 8) | ' ';

    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = blank;
    }
    if (!SECRET_KEEP_VGA_SCREEN_AFTER_CLEAR) {
        for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
            vga_screen[i].letter = ' ';
            vga_screen[i].position.x = 0;
            vga_screen[i].position.y = 0;
        }
    }
    

    set_cursor(0, 0);
}

void scroll(int dir, int times) {
    if (dir == 0) { // Scroll UP
        int start = 0 + times;

        bool copy_val = SECRET_KEEP_VGA_SCREEN_AFTER_CLEAR;
        SECRET_KEEP_VGA_SCREEN_AFTER_CLEAR = true;
        clear(0);
        SECRET_KEEP_VGA_SCREEN_AFTER_CLEAR = copy_val;

        copy_val = KPUTCHAR_CUSTOM_POS;
        KPUTCHAR_CUSTOM_POS = true;

        for (int i = start; i - start < VGA_WIDTH * VGA_HEIGHT; i++) {
            KPUTCHAR_POS_X = i % VGA_WIDTH;
            KPUTCHAR_POS_Y = i / VGA_WIDTH;
            KPUTCHAR_POS_Y -= times;
            if (KPUTCHAR_POS_Y < 0) {
                KPUTCHAR_POS_Y = 0;
            } else if (KPUTCHAR_POS_Y >= VGA_HEIGHT) {
                KPUTCHAR_POS_Y = VGA_HEIGHT - 1;
            }

            kputchar(vga_screen[i].letter, vga_screen[i].color);
        }

        set_to_last_free_spot();
        KPUTCHAR_CUSTOM_POS = copy_val;
    } else {        // Scroll DOWN

    }
}

void kputchar(char c, uint16_t color) {
    if (enable_quiet) return;

    volatile uint16_t* vga = (volatile uint16_t*) VGA;

    if ( color == 0x00 ) {
        color = STANDARD_PRINT_COLOR;
    }

    int idx;
    if (!KPUTCHAR_CUSTOM_POS) {
        idx = cursor_row * VGA_WIDTH + cursor_col;
    } else {
        idx = KPUTCHAR_POS_Y * VGA_WIDTH + KPUTCHAR_POS_X;
    }
    if (c == '\n') {
        // Updates the coordinate data so the cursor moves DOWN and to the LEFT
        vga_screen[idx].letter = '\n';
        vga_screen[idx].color  = color;
        vga_screen[idx].position.x = cursor_col;
        vga_screen[idx].position.y = cursor_row;

        if (!KPUTCHAR_CUSTOM_POS) {
            cursor_row ++;
            cursor_col = 0;
        } else {
            ACK_POS = true;
            KPUTCHAR_POS_Y ++;
            KPUTCHAR_POS_X = 0;
        }
    } else if (c == '\b') {
        cursor_col --;
        if (cursor_col < 0) {
            cursor_col = 0;
            set_to_last_free_spot();
        }
        idx = cursor_row * VGA_WIDTH + cursor_col;
        vga[idx] = (uint16_t) color << 8 | (uint8_t) ' ';
        vga_screen[idx].letter = ' ';
        vga_screen[idx].color  = color;
        vga_screen[idx].position.x = cursor_col;
        vga_screen[idx].position.y = cursor_row;
    } else if (c == '\t') {
        for (int i = 0; i < 4; i++) {
            idx++;
            vga[idx] = (uint16_t) color << 8 | (uint8_t) ' ';
            vga_screen[idx].letter = ' ';
            vga_screen[idx].color  = color;
            vga_screen[idx].position.x = cursor_col;
            vga_screen[idx].position.y = cursor_row;
        }
    } else {
        vga[idx] = (uint16_t) color << 8 | (uint8_t) c;
        vga_screen[idx].letter = c;
        vga_screen[idx].color  = color;
        vga_screen[idx].position.x = cursor_col;
        vga_screen[idx].position.y = cursor_row;
        cursor_col ++;
        // Text wrap
        if (!KPUTCHAR_CUSTOM_POS) {
            if ( cursor_col >= VGA_WIDTH ) {
                cursor_col = 0;
                cursor_row ++;
            }
        } else {
            if ( KPUTCHAR_POS_X >= VGA_WIDTH ) {
                ACK_POS = true;
                KPUTCHAR_POS_X = 0;
                KPUTCHAR_POS_Y ++;
            }
        }
        if ( (!KPUTCHAR_CUSTOM_POS ? cursor_row : KPUTCHAR_POS_Y) >= VGA_HEIGHT - 1 ) {
            //scroll(0, 1);
        }
    }
    // Cursor update
    if (!KPUTCHAR_CUSTOM_POS) {
        set_cursor(cursor_col, cursor_row);
    } else {
        set_cursor(KPUTCHAR_POS_X, KPUTCHAR_POS_Y);
    }
}

void kprint_int(int num, uint16_t color) {
    if (enable_quiet) return;

    if (color == 0x00) {
        color = STANDARD_PRINT_COLOR;
    }
    char* buf = (char*) num;

    if (num == 0) {
        kputchar('0', color);
        return;
    } else if (num < 0) {
        kputchar('-', color);
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
        kputchar(buf[i], color);
    }

}



void print(string msg, uint16_t color ) {
    if (enable_quiet) return;

    volatile uint16_t* vga = (volatile uint16_t*) VGA;

    if ( color == 0x00 ) {
        color = STANDARD_PRINT_COLOR;
    }

    for ( int i = 0; msg[i] != '\0'; i++ ) {
        kputchar(msg[i], color);
    }
}

void kprint_hex(uint32_t hex, uint16_t color) {
    if (enable_quiet) return;

    string buf = "0x00000000";

    for (int i = 9; i >= 2; i--) {
        buf[i] = hex_chars[hex & 0xF];
        hex >>= 4;
    }

    print(buf, color);

}


void vga_init() {
    hw_update_cursor();
}