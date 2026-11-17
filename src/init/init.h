#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "../kernel/libraries/string.h"


void print_kernel_logo(int x, int y);
void print_status(string str, int useOkSign);
void print_crash(int x, int y, string crash_msg);

void init(bool quiet_mode);

#endif