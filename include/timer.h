/*
 * timer.h
 *
 *  Created on: 20.05.2014
 *      Author: andreasbehnke
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <inttypes.h>
#include "hardware.h"

typedef void (*timer_func)(void);

typedef uint16_t timer_time;

extern void timer_init();

extern void timer_start_timeout(uint8_t index, timer_func action, timer_time timeout);

extern void timer_stop_timeout(uint8_t index);

#endif /* TIMER_H_ */
