/*
 * phone.c
 *
 *  Created on: 27.05.2014
 *      Author: andreasbehnke
 */
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "include/debug.h"
#include "include/hardware.h"
#include "include/timer.h"
#include "include/mobile.h"
#include "include/mdevice.h"
#include "include/gsm.h"

#define MOBILE_NO_TIMEOUT      0
#define MOBILE_TIMEOUT_REACHED 1

uint8_t volatile _mobile_timeout = MOBILE_NO_TIMEOUT;

void mobile_init() {
    mdevice_init();
    MOBILE_POWER_ON_DDR |= MOBILE_POWER_ON_PIN;
}

static void mobile_power_switch_timer(void *unused) {
    MOBILE_POWER_ON_PORT &= ~MOBILE_POWER_ON_PIN;
    _mobile_timeout = MOBILE_TIMEOUT_REACHED;
}

static uint8_t mobile_process() {
    while (1) {
        switch (mdevice_process()) {
        case MDEVICE_STATE_READY:
        case MDEVICE_STATE_RESPONSE_READY:
            return MOBILE_READY;
        case MDEVICE_STATE_ERROR:
            return MOBILE_ERROR;
        }
    }
    return MOBILE_ERROR; // compiler food
}

uint8_t mobile_on() {
    uint8_t retry_count = MOBILE_ERROR_RETRY_COUNT;
    uint8_t wait_count;
    while(retry_count-- > 0) {
        uint8_t state;

        // power up mobile phone
        MOBILE_POWER_ON_PORT |= MOBILE_POWER_ON_PIN;
        mdevice_power_on();
        _mobile_timeout = MOBILE_NO_TIMEOUT;
        timer_start_timeout(TIMER_MOBILE_INDEX, mobile_power_switch_timer, NULL, MOBILE_POWER_SWITCH_MS);
        debug_puts("MOBILE: Wait for power on\n\r");
        state = mobile_process();
        while(_mobile_timeout != MOBILE_TIMEOUT_REACHED) {} // wait for release of power switch

        // wait for SIM ready
        if (state == MOBILE_READY) {
            debug_puts("MOBILE: Phone is on\n\r");
            wait_count = MOBILE_WAIT_SIM_READY_COUNT;
            while(wait_count-- > 0) {
                debug_puts("MOBILE: Get pin status\n\r");
                mdevice_tx_get_pin_status();
                state = mobile_process();
                if (state == MOBILE_ERROR || (mdevice_get_pin_status() == MDEVICE_PIN_WAIT_FOR)) {
                    break;
                } else {
                    state = MOBILE_ERROR;
                }
            }
        }

        // enter PIN
        if (state == MOBILE_READY) {
            debug_puts("MOBILE: SIM card ready\n\r");
            debug_puts("MOBILE: Enter pin\n\r");
            uint8_t pin[] = {0x31, 0x32, 0x33, 0x34};
            mdevice_tx_enter_pin(pin);
            state = mobile_process();
            if (state == MOBILE_READY && mdevice_get_pin_status() != MDEVICE_PIN_ACCEPTED) {
                debug_puts("MOBILE: Wrong PIN code\n\r");
                state = MOBILE_ERROR;
            }
        }

        if (state == MOBILE_READY) {
            debug_puts("MOBILE: PIN accepted\n\r");
            mdevice_rc_wait_for_sim_login();
            state = mobile_process();
        }

        if (state == MOBILE_READY) {
            debug_puts("MOBILE: SIM login\n\r");
            mdevice_rc_wait_for_network_status();
            state = mobile_process();
            if (state != MOBILE_ERROR) {
                mdevice_rc_wait_for_network_status(); // wait for second, longer network status
                state = mobile_process();
            }
        }

        if (state == MOBILE_READY) {
            debug_puts("MOBILE: network state received\n\r");
            return state;
        }

        // retry
        debug_puts("MOBILE: Got error, retry again\n\r");
        _delay_ms(MOBILE_POWER_SWITCH_WAIT_TRIGGER_MS);
    }
    return MOBILE_ERROR;
}
uint8_t mobile_send_sms(uint8_t *remote_number_octet, char *message) {
    memcpy(mdevice_sms.remote_number_octet, remote_number_octet, 12);
    mdevice_sms.message_length = strlen(message);
    memcpy(mdevice_sms.message, message, mdevice_sms.message_length);

    uint8_t retry_count = MOBILE_SMS_SEND_RETRY_COUNT;
    while(retry_count-- > 0) {
        uint8_t state;

        // retrieve SMSC
        mdevice_tx_get_smsc();
        state = mobile_process();
        if (state == MOBILE_READY) {
            debug_puts("MOBILE: SMS center number retrieved\n\r");
            mdevice_get_smsc();
            mdevice_sms.encoded_message_length = gsm_pack_7bit(mdevice_sms.encoded_message, mdevice_sms.message, mdevice_sms.message_length);
            mdevice_tx_send_sms();
            state = mobile_process();
        }

        if (state == MOBILE_READY) {
            if (mdevice_get_sms_send_status() == MDEVICE_SMS_SEND_OK) {
                debug_puts("MOBILE: SMS send\n\r");
                return state;
            }
            state = MOBILE_ERROR;
        }

        // retry
        debug_puts("MOBILE: Got error, retry again\n\r");
    }
    return MOBILE_ERROR;
}

uint8_t mobile_receive_sms() {
    uint8_t retry_count = MOBILE_ERROR_RETRY_COUNT;
    while(retry_count-- > 0) {
        uint8_t state;

        // wait for sms
        mdevice_rc_wait_for_sms();
        state = mobile_process();
        if (state == MOBILE_READY) {
            debug_puts("MOBILE: SMS received\n\r");
            mdevice_get_sms();
            gsm_unpack_7bit(mdevice_sms.message, mdevice_sms.encoded_message, mdevice_sms.encoded_message_length);
            // delete sms from phone or sim memory
            mdevice_tx_delete_sms();
            mobile_process(); // we do not care about errors deleting SMS on phone for now

            return state;
        }


        // retry
        debug_puts("MOBILE: Got error, retry again\n\r");
    }
    return MOBILE_ERROR;
}

uint8_t mobile_off() {
    MOBILE_POWER_ON_PORT |= MOBILE_POWER_ON_PIN;
    _delay_ms(MOBILE_POWER_SWITCH_MS);
    MOBILE_POWER_ON_PORT &= ~MOBILE_POWER_ON_PIN;
    return MOBILE_READY;
}
