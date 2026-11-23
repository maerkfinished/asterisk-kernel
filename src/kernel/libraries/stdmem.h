#ifndef STDMEM_H
#define STDMEM_H

#include <stdint.h>
#include <stddef.h>


volatile void *memmove(volatile void *dest, volatile const void *src, size_t sz);

#endif