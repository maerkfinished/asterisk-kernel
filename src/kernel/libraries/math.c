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
int abs(int val) {
    if (val < 0) {
        val = 0 - val;
    }
    return val;
}