/*
 * motion_detection.h
 *
 *  Created on: 03.04.2014
 *      Author: andreasbehnke
 */

#ifndef MOTION_DETECTION_H_
#define MOTION_DETECTION_H_

#include <inttypes.h>

#define MOTION_DETECTION_ACTIVE   1
#define MOTION_DETECTION_INACTIVE 0

/*
 * Initialize motion detection but does not enable interrupts
 */
extern void motion_detection_init();

/*
 * Activate motion detection.
 */
extern void motion_detection_activate(uint8_t threshold);

/*
 * Returns MOTION_DETECTION_ACTIVE, if motion is in progress,
 * returns MOTION_DETECTION_INACTIVE if no motion has been detected
 */
extern uint8_t motion_detection_state();

/*
 * Deactivates motion detection
 */
extern void motion_detection_deactivate();

#endif /* MOTION_intDETECTION_H_ */
