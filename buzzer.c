/*
 * buzzer.c
 *
 *  Created on: 04.04.2014
 *      Author: andreasbehnke
 */
#include "include/hardware.h"
#include "include/buzzer.h"
#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>

#define BUZZER_PERIOD      4
#define BUZZER_HALF_PERIOD 2

volatile uint8_t buzzer_async_state;
volatile uint8_t buzzer_beep_times_count;
volatile uint16_t buzzer_beep_count;
volatile uint16_t buzzer_beep_length;
volatile uint16_t buzzer_pause_length;

void buzzer_init() {
    // set pins to output
    BUZZER_DDR |= BUZZER_PIN_1 | BUZZER_PIN_2;
    // turn buzzer off
    BUZZER_PORT = BUZZER_PORT & ~(BUZZER_PIN_1 | BUZZER_PIN_2);
}

void buzzer_beep(uint8_t times, uint16_t length, uint16_t pause_length) {
    for (uint8_t t = 0; t < times; t++) {
        BUZZER_PORT &= ~BUZZER_PIN_1;
        BUZZER_PORT |= BUZZER_PIN_2;
        for (uint16_t i = 0; i < (length / BUZZER_PERIOD); ++i) {
            BUZZER_PORT ^= BUZZER_PIN_1 | BUZZER_PIN_2;
            _delay_ms(BUZZER_HALF_PERIOD);
            BUZZER_PORT ^= BUZZER_PIN_1 | BUZZER_PIN_2;
            _delay_ms(BUZZER_HALF_PERIOD);
        }
        // turn buzzer off
        BUZZER_PORT = BUZZER_PORT & ~(BUZZER_PIN_1 | BUZZER_PIN_2);
        for (uint16_t i = 0; i < (pause_length / BUZZER_PERIOD); ++i) {
            _delay_ms(BUZZER_PERIOD);
        }
    }
}

void buzzer_async_beep(uint8_t times, uint16_t length, uint16_t pause_length) {
    if (times == 0 || length == 0) {
        return;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        buzzer_async_state = BUZZER_STATE_PLAY;
        buzzer_beep_times_count = times;
        buzzer_beep_count = length;
        buzzer_beep_length = length;
        buzzer_pause_length = pause_length;
        BUZZER_PORT &= ~BUZZER_PIN_1;
        BUZZER_PORT |= BUZZER_PIN_2;
    }
}

uint8_t buzzer_async_get_state() {
    return buzzer_async_state;
}

void buzzer_async_timer() {
    switch (buzzer_async_state) {
        case BUZZER_STATE_PLAY:
            // toggle pin 1 & 2
            BUZZER_PORT ^= BUZZER_PIN_1 | BUZZER_PIN_2;
            buzzer_beep_count--;
            if (buzzer_beep_count == 0) {
                buzzer_beep_times_count--;
                if (buzzer_beep_times_count == 0) {
                    // no more beeps to play, stop asynchronous sound
                    buzzer_async_state = BUZZER_STATE_STOP;
                } else {
                    // switch to pause mode
                    buzzer_async_state = BUZZER_STATE_PAUSE;
                    buzzer_beep_count = buzzer_pause_length;
                }
                // turn buzzer off
                BUZZER_PORT = BUZZER_PORT & ~(BUZZER_PIN_1 | BUZZER_PIN_2);
            }
            break;
        case BUZZER_STATE_PAUSE:
            buzzer_beep_count--;
            if (buzzer_beep_count == 0) {
                // pause is over
                BUZZER_PORT &= ~BUZZER_PIN_1;
                BUZZER_PORT |= BUZZER_PIN_2;
                buzzer_async_state = BUZZER_STATE_PLAY;
                buzzer_beep_count = buzzer_beep_length;
            }
            break;
        default:
            break;
    }
}
