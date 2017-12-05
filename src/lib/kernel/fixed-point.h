//
// Created by youssef on 03/12/17.
//

#ifndef PINTOS_FIXED_POINT_H
#define PINTOS_FIXED_POINT_H

#include <lib/stdint.h>

#define real int64_t
#define converter 1<<14
#define shift 14

/*/
 *
 */


real int_to_real(int64_t );

int64_t  real_to_int(int64_t  );

int64_t  round_real_to_int(real);

real add (real ,real);

real subtract (real ,real);

real multiply (real ,real);

real divide (real ,real);



#endif //PINTOS_FIXED_POINT_H


