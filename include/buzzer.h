/*
 * buzzer.h
 *
 *  Created on: 04.04.2014
 *      Author: andreasbehnke
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include <inttypes.h>

#define BUZZER_STATE_STOP  0
#define BUZZER_STATE_PLAY  1
#define BUZZER_STATE_PAUSE 2

extern void buzzer_init();

/*
 * Produces a synchronous beep. This function blocks until the sound finishes.
 */
extern void buzzer_beep(uint8_t times, uint16_t lengthTone, uint16_t lengthPause);

/*
 * Start asynchronous sound. User needs to call buzzer_async_timer using a timer interrupt.
 */
extern void buzzer_async_beep(uint8_t times, uint16_t lengthTone, uint16_t lengthPause);

/*
 * Return state of asynchronous sound: BUZZER_STATE_PLAY, BUZZER_STATE_PAUSE or BUZZER_STATE_STOP
 */
extern uint8_t buzzer_async_get_state();

/*
 * Needs to be called by timer interrupt routine for asynchronous sound
 */
extern void buzzer_async_timer();

#endif /* BUZZER_H_ */
