/*
 * phone.h
 *
 *  Created on: 03.05.2014
 *      Author: andreasbehnke
 */

#ifndef PHONE_H_
#define PHONE_H_

#include <stdio.h>

#include <inttypes.h>
#include "hardware.h"

#define PHONE_STATE_OFF 0x00
#define PHONE_STATE_ON  0x01

extern void phone_init();

extern uint8_t phone_process(FILE *debug);

#endif /* PHONE_H_ */
