#ifndef DRIVER_H
#define DRIVER_H

#include "../kernel/libraries/string.h"

typedef struct {
    const string name;
    const string version;
    int (*init)();
} driver_t;
typedef struct {
    const string name;
    const string version;
    void (*init)();
} void_driver_t;

#endif