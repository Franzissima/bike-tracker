/*
 * hardware.h
 *
 *  Created on: 27.04.2014
 *      Author: andreasbehnke
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

/* buzzer */

#define BUZZER_PIN_1       (1<<PA0)
#define BUZZER_PIN_2       (1<<PA1)
#define BUZZER_DDR         DDRA
#define BUZZER_PORT        PORTA

/* led */
#define LED_PIN            PB0
#define LED_DDR            DDRB
#define LED_PORT           PORTB

#endif /* HARDWARE_H_ */
