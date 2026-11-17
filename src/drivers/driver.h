#ifndef DRIVER_H
#define DRIVER_H

#include "../kernel/libraries/string.h"

typedef struct {
    const string name;
    const string version;
    void (*init)();
} driver_t;

#endif