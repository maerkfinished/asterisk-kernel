#include <stdint.h>
#include "math.h"



int clamp(int val, int start, int end) {
    
    if (val < start) {
        val = start;
    }
    if (val > end) {
        val = end;
    }

    return val;
}