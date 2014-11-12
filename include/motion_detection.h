/*
 * motion_detection.h
 *
 *  Created on: 03.04.2014
 *      Author: andreasbehnke
 */

#ifndef MOTION_DETECTION_H_
#define MOTION_DETECTION_H_

#include <inttypes.h>

#define MOTION_DETECTION_STATE_SLEEP          0
#define MOTION_DETECTION_STATE_WAIT           1
#define MOTION_DETECTION_STATE_NO_MOTION      2
#define MOTION_DETECTION_STATE_MOTION         3

extern volatile uint8_t motion_detection_state;

#define motion_detection_wait() while(motion_detection_state == MOTION_DETECTION_STATE_WAIT) {}

extern void motion_detection_init();

extern void motion_detection_enable_watchdog();

#endif /* MOTION_intDETECTION_H_ */
