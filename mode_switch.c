/*
 * mode_switch.c
 *
 *  Created on: 04.06.2014
 *      Author: andreasbehnke
 */

#include "include/debug.h"
#include "include/mode_switch.h"
#include "include/timer.h"
#include "include/buzzer.h"
#include <avr/interrupt.h>
#include <stddef.h>

volatile uint8_t mode_switch_state = MODE_SWITCH_STATE_SLEEP;

volatile uint8_t mode_switch_value = 0;

static void mode_switch_debounce_pressed(void *unused);

static void mode_switch_debounce_released(void *unused);

static void mode_switch_next();

static void mode_switch_timer(void *unused);

ISR(MODE_SWITCH_VECT) {
    debug_puts("MODE SWITCH: Wake up\n\r");
    mode_switch_state = MODE_SWITCH_STATE_WAIT;
    // one shot interrupt
    PCICR &= ~MODE_SWITCH_PCIE;
    timer_start_timeout(TIMER_MODE_SWITCH_INDEX, mode_switch_debounce_pressed, NULL, MODE_SWITCH_DEBOUNCE_MS);
}

static void mode_switch_debounce_pressed(void *unused) {
    if ((MODE_SWITCH_PIN & MODE_SWITCH_BIT) > 0) {
        if (mode_switch_value != 0) {
            mode_switch_value = 0;
        }
        debug_puts("MODE SWITCH: Key pressed\n\r");
        mode_switch_next();
    } else {
        mode_switch_state = MODE_SWITCH_STATE_NO_SELECTION;
        return;
    }
}

static void mode_switch_next() {
    uint16_t timeout = 0;
    if (mode_switch_value == 0) {
        buzzer_beep(1, MODE_SWITCH_LONG_BEEP_MS, 0);
        timeout = MODE_SWITCH_LONG_BEEP_MS;
    } else {
        uint8_t times = mode_switch_value + 1;
        buzzer_beep(times , MODE_SWITCH_SHORT_BEEP_MS, MODE_SWITCH_BEEP_PAUSE_MS);
        timeout += (MODE_SWITCH_SHORT_BEEP_MS + MODE_SWITCH_BEEP_PAUSE_MS) * times;
    }
    timeout += MODE_SWITCH_TIMEOUT_MS;
    timer_start_timeout(TIMER_MODE_SWITCH_INDEX, mode_switch_timer, NULL, timeout);
}

static void mode_switch_debounce_released(void *unused) {
    if ((MODE_SWITCH_PIN & MODE_SWITCH_BIT) == 0) {
        debug_puts("MODE SWITCH: Key released\n\r");
        mode_switch_state = MODE_SWITCH_STATE_NEW_SELECTION;
    } else {
        mode_switch_timer(NULL);
    }
}

static void mode_switch_timer(void *unused) {
    if ((MODE_SWITCH_PIN & MODE_SWITCH_BIT) == 0) {
        timer_start_timeout(TIMER_MODE_SWITCH_INDEX, mode_switch_debounce_released, NULL, MODE_SWITCH_DEBOUNCE_MS);
        return;
    }
    mode_switch_value++;
    if (mode_switch_value > MODE_SWITCH_MAX_MODE) {
        mode_switch_value = 0;
    }
    mode_switch_next();
}


void mode_switch_init() {
    MODE_SWITCH_PCMSK |= MODE_SWITCH_BIT;
}

void mode_switch_enable_watchdog() {
    mode_switch_state = MODE_SWITCH_STATE_SLEEP;
    PCICR |= MODE_SWITCH_PCIE;
}
