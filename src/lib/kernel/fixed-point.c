//
// Created by youssef on 03/12/17.
//

#include "fixed-point.h"


real int_to_real(int x) {
    return  x*converter;
}

int real_to_int(real f) {
    return f/converter;
}

int round_real_to_int(real f) {
   // if (f )
}

real add (real x,real y){
    return x+y;
}

real subtract (real x ,real y){
    return  x-y;
}

real multiply (real x,real y){
    return  x*y/converter;
}

real divide (real x ,real y){
    return x*converter/y;
}

