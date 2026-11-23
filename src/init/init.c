#include "init.h"
#include "../drivers/vga.h"
#include "../drivers/ps2kbd.h"

extern driver_t PS2_KEYBD_DRIVER;


void print_kernel_logo(int x, int y) {
    update_cursor(x, y);

    printchar('\n', 0);
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
    update_cursor(x, y);

    printchar('\n', 0xF0);
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
void print_status(string str, int sign) {
    if (!print_status_first_time) {
        printchar('\n', 0);
    } else {
        print_status_first_time = false;
    }
    if (sign == 0) {
        print("[ ", 0);
        print("OK", 0x0A);
        print(" ] ", 0);
    }
    else if (sign == 1) {
        print("[ ", 0);
        print("INF", 0x08);
        print(" ] ", 0);
    }
    else if (sign == 2) {
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

    quiet_mode = quiet_mode;
    configure_cursor(!quiet_mode);
    driver_t *int_drivers[] = {&PS2_KEYBD_DRIVER, NULL};

    print_status("Loading drivers..", 1);
    for (int i = 0; int_drivers[i]; i++) {
        print_status("Starting driver ", 1);
        print(int_drivers[i]->name, 0);
        print("..", 0);
        int result = int_drivers[i]->init();
        if (result > 0) {
            print_status("Failed", 3);
        } else print_status("Success", 0);
    }
    
    while (1) {
        printchar(kbd_getchar(), 0);
    }


    while (1) {
        __asm__ volatile ("hlt");
    }

}