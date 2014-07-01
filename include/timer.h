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

typedef void (*timer_func)(void *data);

typedef void (*trigger_func)(void);

typedef uint16_t timer_time;

extern void timer_init();

extern void timer_wait_finish();

extern void timer_start_timeout(uint8_t index, timer_func action, void *data, timer_time timeout);

extern void timer_stop_timeout(uint8_t index);

extern void timer_add_trigger(uint8_t index, trigger_func trigger);

extern void timer_remove_trigger(uint8_t index);

#endif /* TIMER_H_ */
