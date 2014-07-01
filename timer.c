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

typedef struct
{
  timer_time when;
  timer_func action;
  void *data;
} timer_entry;

timer_entry *timer_entries[TIMER_COUNT];

trigger_func timer_triggers[TRIGGER_COUNT];

#define TIMER_STATE_STOPPED 0
#define TIMER_STATE_RUNNING 1

volatile uint8_t timer_state = TIMER_STATE_STOPPED;

static void timer_start() {
#ifdef DEBUG
    led_on();
#endif
    TIMSK0 |= (1 << OCIE0A);
    timer_state = TIMER_STATE_RUNNING;
}

static void timer_stop() {
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
    for (int i = 0; i < TRIGGER_COUNT; ++i) {
        timer_triggers[i] = NULL;
    }
    timer_state = TIMER_STATE_STOPPED;
    TCCR0A = (1 << WGM01);
    TCCR0B = TIMER_PRESCALE;
    TCNT0 = 0;
    OCR0A = TIMER_COMPARE;
}

ISR(TIMER0_COMPA_vect)
{
    for (uint8_t i = 0; i < TIMER_COUNT; ++i) {
        timer_entry *entry = timer_entries[i];
        if (entry->when != 0) {
            entry->when--;
            if (entry->when == 0) {
                entry->action(entry->data);
            }
        }
    }
    for (int i = 0; i < TRIGGER_COUNT; ++i) {
        trigger_func trigger = timer_triggers[i];
        if (trigger != NULL) {
            trigger();
        }
    }
    // test if there are remaining triggers or timers after processing all
    uint8_t active = 0;
    for (uint8_t i = 0; i < TIMER_COUNT; ++i) {
        if (timer_entries[i]->when != 0) {
            active++;
        }
    }
    for (int i = 0; i < TRIGGER_COUNT; ++i) {
        if (timer_triggers[i] != NULL) {
            active++;
        }
    }
    if (active == 0) {
        timer_stop();
    }
}

void timer_wait_finish() {
    while(timer_state == TIMER_STATE_RUNNING) {}
}

void timer_start_timeout(uint8_t index, timer_func action, void *data, timer_time timeout) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timer_entry *entry = timer_entries[index];
        entry->when = timeout;
        entry->action = action;
        entry->data = data;
        if (timer_state == TIMER_STATE_STOPPED) {
            timer_start();
        }
    }
}

void timer_stop_timeout(uint8_t index) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timer_entry *entry = timer_entries[index];
        entry->when = 0;
    }
}

void timer_add_trigger(uint8_t index, trigger_func trigger) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timer_triggers[index] = trigger;
        if (timer_state == TIMER_STATE_STOPPED) {
            timer_start();
        }
    }
}

void timer_remove_trigger(uint8_t index) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timer_triggers[index] = NULL;
    }
}
