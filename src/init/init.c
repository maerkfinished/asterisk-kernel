#include "init.h"
#include "../drivers/driver.h"
#include "../drivers/vga.h"
#include "../drivers/ps2kbd.h"

extern driver_t PS2_KEYBD_DRIVER;


void print_kernel_logo(int x, int y) {
    set_cursor(x, y);

    kputchar('\n', 0);
    print("             *                             \n", 0);
    print("            *%*                            \n", 0);
    print("          *%#$#%*                          \n", 0);
    print("        *%#$&&&$#%*                        \n", 0);
    print("      *$$&&&@@@&&&$$*                      \n", 0);
    print("   *%$$$&&@@@@@@@&&$$$%*                   \n", 0);
    print("      *$$&&&@@@&&&$$*                      \n", 0);
    print("        *%$#&&&#$%*                        \n", 0);
    print("       %$&$%   %$&$%                       \n", 0);
    print("       ^%%^     ^%%^                    ", 0); print("x86\n", 0);
    print("===========================================\n", 0);
    print("    Asterisk Kernel    |    version 0.3    \n", 0);
    print("===========================================\n", 0);
}

void print_crash(int x, int y, string crash_msg) {
    clear(0xF0);
    set_cursor(x, y);

    kputchar('\n', 0xF0);
    print("             *                             \n", 0xF0);
    print("            *%*                            \n", 0xF0);
    print("          *%#$#%*                          \n", 0xF0);
    print("        *%#$&&&$#%*                        \n", 0xF0);
    print("      *$$&&&@@@&&&$$*                      \n", 0xF0);
    print("   *%$$$&&@@@@@@@&&$$$%*                   \n", 0xF0);
    print("      *$$&&&@@@&&&$$*                      \n", 0xF0);
    print("        *%$#&&&#$%*                        \n", 0xF0);
    print("       %$&$%   %$&$%                       \n", 0xF0);
    print("       ^%%^     ^%%^                    ", 0xF0); print("x86\n", 0xF0);
    print("===========================================\n", 0xF0);
    print("  Asterisk Kernel  | version 0.3 |  CRASH  \n", 0xF0);
    print("===========================================\n", 0xF0);
    print("CRASH INFO: ", 0xF0);
    print(crash_msg, 0xF0);
}

bool print_status_first_time = true;
void print_status(string str, int useOkSign) {
    if (!print_status_first_time) {
        kputchar('\n', 0);
    } else {
        print_status_first_time = false;
    }
    if (useOkSign == 0) {
        print("[ ", 0);
        print("OK", 0x0A);
        print(" ] ", 0);
    }
    else if (useOkSign == 1) {
        print("[ ", 0);
        print("INF", 0x08);
        print(" ] ", 0);
    }
    else if (useOkSign == 2) {
        print("[ ", 0);
        print("WARN", 0x0E);
        print(" ] ", 0);
    }
    else {
        print("[ ", 0);
        print("ERR", 0x0C);
        print(" ] ", 0);
    }
    print(str, 0);
}


void init(bool quiet_mode) {

    enable_quiet = quiet_mode;
    configure_cursor(!quiet_mode);

    print_status("Loading drivers..", 1);
    print_status("Starting driver ", 0);
    print(PS2_KEYBD_DRIVER.name, 0);
    print("..", 0);
    PS2_KEYBD_DRIVER.init();

    print("\nNow you can type here: ", 0x0A);
    while (1) {
        kputchar(kbd_getchar(), 0);
    }


    while (1) {
        __asm__ volatile ("hlt");
    }

}