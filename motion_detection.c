/*
 * motion_detection.c
 *
 *  Created on: 10.04.2014
 *      Author: andreasbehnke
 */

#include "include/debug.h"
#include "include/motion_detection.h"
#include "include/timer.h"
#include <avr/interrupt.h>
#include <stddef.h>

#define MOTION_DETECTION_PULSE_INC    5
#define MOTION_DETECTION_THRESHOLD   25
#define MOTION_DETECTION_TIMEOUT_MS 500

static volatile uint8_t motion_detection_pulse_count = 0;

volatile uint8_t motion_detection_state = MOTION_DETECTION_STATE_NO_MOTION;

static void motion_detection_timer(void *unused);

ISR(MOTION_DETECTION_VECT) {
    motion_detection_pulse_count += MOTION_DETECTION_PULSE_INC;
    // disable one shot interrupt
    PCICR &= ~MOTION_DETECTION_PCIE;
    if (motion_detection_state == MOTION_DETECTION_STATE_SLEEP) {
        motion_detection_state = MOTION_DETECTION_STATE_WAIT;
        timer_start_timeout(TIMER_MOT_DETECTION_INDEX, motion_detection_timer, NULL, MOTION_DETECTION_TIMEOUT_MS);
    }
}

static void motion_detection_timer(void *unused) {
    if (motion_detection_pulse_count > 0) {
        motion_detection_pulse_count--;
        if (motion_detection_pulse_count > MOTION_DETECTION_THRESHOLD) {
            motion_detection_state = MOTION_DETECTION_STATE_MOTION;
            return; // motion detected, do not trigger one shot interrupt again
        }
    } else {
        // no motion detected, do not trigger one shot interrupt again
        motion_detection_state = MOTION_DETECTION_STATE_NO_MOTION;
        return;
    }
    // enable one shot interrupt...
    PCICR |= MOTION_DETECTION_PCIE;
    // ...and wait again
    timer_start_timeout(TIMER_MOT_DETECTION_INDEX, motion_detection_timer, NULL, MOTION_DETECTION_TIMEOUT_MS);
}

extern void motion_detection_init() {
    MOTION_DETECTION_PORT &= ~MOTION_DETECTION_BIT; // disable pull up resistor
    MOTION_DETECTION_PCMSK |= MOTION_DETECTION_BIT; // enable PCINT pin for motion detection
}

extern void motion_detection_enable_watchdog() {
    motion_detection_state = MOTION_DETECTION_STATE_SLEEP;
    motion_detection_pulse_count = 0; // reset pulse counter
    PCIFR |= MOTION_DETECTION_PCIF;
    PCICR |= MOTION_DETECTION_PCIE;
}
