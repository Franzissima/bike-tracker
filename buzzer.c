/*
 * buzzer.c
 *
 *  Created on: 04.04.2014
 *      Author: andreasbehnke
 */
#include "include/hardware.h"
#include "include/debug.h"
#include "include/buzzer.h"
#include "include/timer.h"
#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <stddef.h>

volatile uint8_t buzzer_beep_times_count;
volatile uint16_t buzzer_beep_length;
volatile uint16_t buzzer_pause_length;

void buzzer_init() {
    // set pins to output
    BUZZER_DDR |= BUZZER_PIN_1 | BUZZER_PIN_2;
    // turn buzzer off
    BUZZER_PORT &= ~(BUZZER_PIN_1 | BUZZER_PIN_2);
}

static void buzzer_stop_beep(void *data);

static void buzzer_toggle() {
    // toggle pin 1 & 2
    BUZZER_PORT ^= BUZZER_PIN_1 | BUZZER_PIN_2;
}

static void buzzer_start_beep(void *data) {
    BUZZER_PORT &= ~BUZZER_PIN_1;
    BUZZER_PORT |= BUZZER_PIN_2;
    timer_start_timeout(TIMER_BUZZER_INDEX, buzzer_stop_beep, NULL, buzzer_beep_length);
    timer_add_trigger(TRIGGER_BUZZER_INDEX, buzzer_toggle);
    debug_puts("BUZZER: started beep\n\r");
}

static void buzzer_stop_beep(void *data) {
    timer_remove_trigger(TRIGGER_BUZZER_INDEX);
    // turn buzzer off
    BUZZER_PORT &= ~(BUZZER_PIN_1 | BUZZER_PIN_2);
    buzzer_beep_times_count--;
    debug_puts("BUZZER: stopped beep\n\r");
    if (buzzer_beep_times_count > 0) {
        timer_start_timeout(TIMER_BUZZER_INDEX, buzzer_start_beep, NULL, buzzer_pause_length);
        debug_puts("BUZZER: triggered timeout start beep\n\r");
    }
}

void buzzer_beep(uint8_t times, uint16_t length, uint16_t pause_length) {
    if (times == 0 || length == 0) {
        return;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        buzzer_beep_times_count = times;
        buzzer_beep_length = length;
        buzzer_pause_length = pause_length;
        buzzer_start_beep(NULL);
    }
}
