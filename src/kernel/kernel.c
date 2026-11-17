#include "../init/init.h"
#include "../drivers/driver.h"
#include "../drivers/vga.h"
#include "kernel_config.h"

extern driver_t vga;

void kernel_main() {

    vga.init();

    if (QUIET) {
            clear(0);
            configure_cursor(false);
    } else {
        print_kernel_logo(cursor_col, cursor_row);
    }

    init(QUIET_INIT);
    
}
