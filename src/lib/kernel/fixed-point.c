//
// Created by youssef on 03/12/17.
//

#include "fixed-point.h"
#include "lib/stdio.h"

real int_to_real(int64_t  x) {
    return x * converter;
}

int64_t  real_to_int(int64_t  f) {
    return f >> shift;
}

int64_t  round_real_to_int(real f) {
    // if (f )
}

real add(real x, real y) {
    return x + y;
}

real subtract(real x, real y) {
    return x - y;
}

real multiply(real x, real y) {
    return (x * y) >> shift;
}

real divide(real x, real y) {
    real r = x * converter;
    return ((x * converter) / y);
}

