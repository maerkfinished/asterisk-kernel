#include "driver.h"
#include "ps2kbd.h"
#include "vga.h"
#include "../kernel/libraries/hwreg.h"
#include "../kernel/libraries/string.h"

driver_t PS2_KEYBD_DRIVER = {
    .name    = "PS/2 Keyboard Driver",
    .version = "0.3.1",
    .init    = ps2_keyboard_init
};

static bool initialized = false;


static dict_hex hex_to_char = {
    // Characters
    {0x00000001, ' '},
    {0x00000029, '`'}, {0x00000002, '1'}, {0x00000003, '2'}, {0x00000004, '3'}, {0x00000005, '4'}, {0x00000006, '5'},
    {0x00000007, '6'}, {0x00000008, '7'}, {0x00000009, '8'}, {0x0000000A, '9'}, {0x0000000B, '0'}, {0x0000000C, '-'},
    {0x0000000D, '='}, {0x0000000E, '\b'},
    {0x0000000F, '\t'},
    {0x00000010, 'q'}, {0x00000011, 'w'}, {0x00000012, 'e'}, {0x00000013, 'r'}, {0x00000014, 't'}, {0x00000015, 'y'},
    {0x00000016, 'u'}, {0x00000017, 'i'}, {0x00000018, 'o'}, {0x00000019, 'p'}, {0x0000001A, '['}, {0x0000001B, ']'},
    {0x0000001C, '\n'},
    {0x0000001E, 'a'}, {0x0000001F, 's'}, {0x00000020, 'd'}, {0x00000021, 'f'}, {0x00000022, 'g'}, {0x00000023, 'h'},
    {0x00000024, 'j'}, {0x00000025, 'k'}, {0x00000026, 'l'}, {0x00000027, ';'}, {0x00000028, '\''}, {0x0000002B, '\\'},
    {0x0000002C, 'z'}, {0x0000002D, 'x'}, {0x0000002E, 'c'}, {0x0000002F, 'v'}, {0x00000030, 'b'},
    {0x00000031, 'n'}, {0x00000032, 'm'}, {0x00000033, ','}, {0x00000034, '.'}, {0x000000035, '/'}, {0x00000037, '*'},
    {0x00000039, ' '}, {0x0000004E, '+'}, {0x0000004A, '-'},

    // Arrow keys
    {0x0000004B, '\1'}, {0x00000048, '\2'}, {0x00000050, '\3'}, {0x0000004D, '\4'}
};


static int get_dict_len(dict_hex dict) {
    int i = 1;
    while (dict[i].key) {
        i++;
    }
    return i - 1;
}

static char get_char_from_dict(uint32_t hex, dict_hex dict) {
    int i = 0;
    int dict_len = get_dict_len(dict);
    while (((uint32_t) dict[i].key != hex) && i < dict_len) {
        i++;
    }
    return dict[i].val;
}

static inline void kbd_cooldown() {
    while (inb(REG_KBD_STAT) & 0x02);
}


bool SHIFT_PRESSED       = false;
bool CAPS_LOCK           = false;
bool OTHER_SHIFT_PRESSED = false;
char kbd_getchar() {
    if (!initialized) return '\0';
    while (1) {
        uint8_t status = inb(REG_KBD_STAT);

        if ((status & 0x01) && !(status & 0x20)) {
            uint8_t scancode = inb(REG_KBD_DATA);

            if ((scancode == 0x0000002A || scancode == 0x00000036)) {
                if (!SHIFT_PRESSED) {
                    SHIFT_PRESSED = true;
                } else {
                    OTHER_SHIFT_PRESSED = true;
                }
            } else if (scancode == 0x0000003A) {
                kbd_cooldown();
                outb(REG_KBD_DATA, 0xED);
                kbd_cooldown();
                outb(REG_KBD_DATA, CAPS_LOCK ? 0x00 : 0x04);
                CAPS_LOCK = !CAPS_LOCK;
            }

            if (scancode & 0x80) {
                if (scancode == 0x000000AA || scancode == 0x000000B6) {
                    if (OTHER_SHIFT_PRESSED) {
                        OTHER_SHIFT_PRESSED = false;
                    } else {
                        SHIFT_PRESSED = false;
                    }
                }
                continue;
            }
            
            char c = get_char_from_dict(scancode, hex_to_char);
            //printhex(scancode, 0);
            if (c) {
                if ((SHIFT_PRESSED || CAPS_LOCK) && !iscntrl(c)) {
                    c = upper(c);
                }
                return c;
            }
        }
    }
}


int ps2_keyboard_init() {
    if (initialized) return 0;

    outb(REG_KBD_DATA, 0xF5);
    outb(REG_KBD_DATA, 0xEE);
    uint16_t keyboard = inb(REG_KBD_DATA);
    if (keyboard == 0xFF || keyboard == 0x00) {
        return 1;
    }
    
    SHIFT_PRESSED = 0;
    while (inb(REG_KBD_STAT) & 0x01) {
        inb(REG_KBD_DATA);
    }

    outb(REG_KBD_DATA, 0xF4);

    initialized = true;
    return 0;
}