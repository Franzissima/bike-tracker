/*
 * mobile.h
 *
 *  Created on: 19.05.2014
 *      Author: andreasbehnke
 */

#ifndef MOBILE_H_
#define MOBILE_H_

#include <inttypes.h>
#include "mdevice.h"

extern void mobile_init();

#define MOBILE_READY   0
#define MOBILE_ERROR   255

extern void mobile_init();

/*
 All functions return MOBILE_READY or MOBILE_ERROR
 */

extern uint8_t mobile_on();

extern uint8_t mobile_send_sms(uint8_t *remote_number_octet, char *message);

extern uint8_t mobile_receive_sms();

extern uint8_t mobile_off();

#endif /* MOBILE_H_ */
