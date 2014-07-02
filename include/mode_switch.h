/*
 * mode_switch.h
 *
 *  Created on: 04.06.2014
 *      Author: andreasbehnke
 */

#ifndef MODE_SWITCH_H_
#define MODE_SWITCH_H_

#include <inttypes.h>

#define MODE_SWITCH_STATE_SLEEP          0
#define MODE_SWITCH_STATE_WAIT           1
#define MODE_SWITCH_STATE_NO_SELECTION   2
#define MODE_SWITCH_STATE_NEW_SELECTION  3

extern volatile uint8_t mode_switch_state;

#define mode_switch_wait() while(mode_switch_state == MODE_SWITCH_STATE_WAIT) {}

extern volatile uint8_t mode_switch_value;

extern void mode_switch_init();

extern void mode_switch_enable_watchdog();

#endif /* MODE_SWITCH_H_ */
