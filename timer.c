/*
 * timer.c
 *
 *  Created on: 20.05.2014
 *      Author: andreasbehnke
 */

#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "include/timer.h"
#include "include/led.h"

#define TIMER_STATE_STOPPED 0
#define TIMER_STATE_RUNNING 1

typedef struct
{
  timer_time when;
  timer_func action;
} timer_entry;

timer_entry *timer_entries[TIMER_COUNT];

uint8_t timer_state = TIMER_STATE_STOPPED;


void _timer_start() {
#ifdef DEBUG
    led_on();
#endif
    TIMSK0 |= (1 << OCIE0A);
    timer_state = TIMER_STATE_RUNNING;
}

void _timer_stop() {
#ifdef DEBUG
    led_off();
#endif
    TIMSK0 &= ~(1 << OCIE0A);
    timer_state = TIMER_STATE_STOPPED;
}

void timer_init() {
    for (int i = 0; i < TIMER_COUNT; ++i) {
        timer_entries[i] = malloc(sizeof(timer_entry));
        timer_entries[i]->when = 0;
    }
    timer_state = TIMER_STATE_STOPPED;
    TCCR0A = (1 << WGM01);
    TCCR0B = TIMER_PRESCALE;
    TCNT0 = 0;
    OCR0A = TIMER_COMPARE;
}

ISR (TIMER0_COMPA_vect)
{
    uint8_t active = 0;
    for (uint8_t i = 0; i < TIMER_COUNT; ++i) {
        timer_entry *entry = timer_entries[i];
        if (entry->when != 0) {
            active++;
            entry->when--;
            if (entry->when == 0) {
                entry->action();
                active--;
            }
        }
    }
    if (active == 0) {
        _timer_stop();
    }
}

void timer_start_timeout(uint8_t index, timer_func action, timer_time timeout) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timer_entry *entry = timer_entries[index];
        entry->when = timeout;
        entry->action = action;
        if (timer_state == TIMER_STATE_STOPPED) {
            _timer_start();
        }
    }
}

void timer_stop_timeout(uint8_t index) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timer_entry *entry = timer_entries[index];
        entry->when = 0;
    }
}
