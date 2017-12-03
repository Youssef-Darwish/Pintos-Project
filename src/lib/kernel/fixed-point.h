//
// Created by youssef on 03/12/17.
//

#ifndef PINTOS_FIXED_POINT_H
#define PINTOS_FIXED_POINT_H
#define real int
#define converter 1<<14

/*/
 *
 */


real int_to_real(int );

int real_to_int(real );

int round_real_to_int(real);

real add (real ,real);

real subtract (real ,real);

real multiply (real ,real);

real divide (real ,real);



#endif //PINTOS_FIXED_POINT_H


