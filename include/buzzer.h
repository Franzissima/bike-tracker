/*
 * buzzer.h
 *
 *  Created on: 04.04.2014
 *      Author: andreasbehnke
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include <inttypes.h>

extern void buzzer_init();

extern void buzzer_beep(uint8_t times, uint16_t lengthTone, uint16_t lengthPause);

#endif /* BUZZER_H_ */
