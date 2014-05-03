/*
 * led.h
 *
 *  Created on: 03.05.2014
 *      Author: andreasbehnke
 */

#ifndef LED_H_
#define LED_H_

#include <avr/io.h>
#include "hardware.h"

extern void led_init();

extern void led_on();

extern void led_off();

#endif /* LED_H_ */
