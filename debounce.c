/*
 * debounce.c
 *
 *  Created on: 07.04.2014
 *      Author: andreasbehnke
 */

#include "include/debounce.h"
#include <util/delay.h>
#include <avr/io.h>

#define DEBOUNCE_TIME_US       300
#define DEBOUNCE_SWITCH_MASK   (1<<PA0) | (1<<PA1)
#define DEBOUNCE DDR           DDRA
#define DEBOUNCE_PORT          PORTA
#define DEBOUNCE_PIN           PINA

void debounce_init() {

}

uint8_t debounce_read_switches() {
    uint8_t activatedSwitches = DEBOUNCE_PIN;
    activatedSwitches &= DEBOUNCE_SWITCH_MASK;
    _delay_us(DEBOUNCE_TIME_US);
    activatedSwitches &= DEBOUNCE_PIN;
    return activatedSwitches;
}
