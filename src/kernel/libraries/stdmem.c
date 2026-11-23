#include "stdmem.h"


volatile void *memmove(volatile void *dest, volatile const void *src, size_t sz) {
    uint8_t *d = (uint8_t*) dest;
    const uint8_t *s = (const uint8_t*) src;

    if (d == s || sz == 0) {
        return dest;
    }

    if (d < s) {
        for (size_t i = 0; i < sz; i++) {
            d[i] = s[i];
        }
    } else {
        size_t i = sz;
        while (i--) {
            d[i] = s[i];
        }
    }

    return dest;
}