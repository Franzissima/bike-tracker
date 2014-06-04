/*
 * led.c
 *
 *  Created on: 03.05.2014
 *      Author: andreasbehnke
 */

#include "include/led.h"

inline void led_init() {
    LED_DDR |= LED_PIN;
}

inline void led_on() {
    LED_PORT |= LED_PIN;
}

inline void led_off() {
    LED_PORT &= ~LED_PIN;
}

inline void led_toggle() {
    LED_PORT ^= LED_PIN;
}
