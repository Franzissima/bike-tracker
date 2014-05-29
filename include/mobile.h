/*
 * mobile.h
 *
 *  Created on: 19.05.2014
 *      Author: andreasbehnke
 */

#ifndef MOBILE_H_
#define MOBILE_H_

#include <inttypes.h>

extern void mobile_init();

#define MOBILE_READY   0
#define MOBILE_ERROR   255

extern void mobile_init();

/*
 Return codes are MOBILE_READY or MOBILE_ERROR
 */
extern uint8_t mobile_on();

/*
 Return codes are MOBILE_READY or MOBILE_ERROR
 */
extern uint8_t mobile_off();

#endif /* MOBILE_H_ */
