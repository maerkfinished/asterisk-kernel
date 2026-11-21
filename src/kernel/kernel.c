#include "../init/init.h"
#include "../drivers/driver.h"
#include "../drivers/vga.h"
#include "kernel_config.h"

extern driver_t VGA_DRIVER;

void kernel_main() {

    VGA_DRIVER.init();

    if (QUIET) {
            clear(0);
            configure_cursor(false);
    } else {
        print_kernel_logo(0, cursor_y);
    }

    init(QUIET_INIT);
    
}
