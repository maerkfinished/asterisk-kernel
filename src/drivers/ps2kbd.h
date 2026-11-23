#ifndef PS2_KEYBD
#define PS2_KEYBD

#define REG_KBD_DATA 0x60
#define REG_KBD_STAT 0x64

typedef struct {
    const int key;
    int val;
} dict_hex[];



char kbd_getchar();

int ps2_keyboard_init();

#endif