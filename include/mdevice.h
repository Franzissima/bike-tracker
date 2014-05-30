/*
 * phone.h
 *
 *  Created on: 03.05.2014
 *      Author: andreasbehnke
 */

#ifndef MDEVICE_H_
#define MDEVICE_H_

#include <stdio.h>

#include <inttypes.h>
#include "hardware.h"

#define MDEVICE_STATE_OFF               0
#define MDEVICE_STATE_WAIT_FOR_POWER_ON 1
#define MDEVICE_STATE_READY             2
#define MDEVICE_STATE_WAIT_FOR_ACK      3
#define MDEVICE_STATE_WAIT_FOR_RESPONSE 4
#define MDEVICE_STATE_RESPONSE_READY    5
#define MDEVICE_STATE_ERROR             255

#define MDEVICE_PIN_ACCEPTED            0
#define MDEVICE_PIN_SIM_CARD_NOT_READY  1
#define MDEVICE_PIN_WRONG_PIN_CODE      2
#define MDEVICE_PIN_CHANGE_OK           3
#define MDEVICE_PIN_WAIT_FOR            4
#define MDEVICE_PIN_UNKNOWN             255

typedef struct {
    uint8_t *smsc_octet;
    uint8_t *remote_number_octet;
    uint8_t message_length;
    uint8_t encoded_message_length;
    uint8_t *encoded_message;
} MDEVICE_SMS_DATA;

extern void mdevice_init();

/* returns:
MDEVICE_STATE_OFF
MDEVICE_STATE_WAIT_FOR_POWER_ON
MDEVICE_STATE_READY
MDEVICE_STATE_WAIT_FOR_ACK
MDEVICE_STATE_WAIT_FOR_RESPONSE
MDEVICE_STATE_RESPONSE_READY
MDEVICE_STATE_ERROR
*/
extern uint8_t mdevice_process();

extern void mdevice_power_on();

extern void mdevice_tx_get_status();

extern uint8_t mdevice_get_status();

extern void mdevice_tx_get_hdw_version();

extern uint8_t *mdevice_get_hdw_version();

extern void mdevice_rc_wait_for_network_status();

extern void mdevice_tx_get_pin_status();

/* returns:
MDEVICE_PIN_ACCEPTED
MDEVICE_PIN_SIM_CARD_NOT_READY
MDEVICE_PIN_WRONG_PIN_CODE
MDEVICE_PIN_CHANGE_OK
MDEVICE_PIN_WAIT_FOR
MDEVICE_PIN_UNKNOWN
 */
extern uint8_t mdevice_get_pin_status();

extern void mdevice_tx_enter_pin(uint8_t pin[4]);

extern uint8_t mdevice_enter_pin_result();

extern void mdevice_tx_get_smsc();

extern uint8_t *mdevice_get_smsc();

extern void mdevice_tx_send_sms(MDEVICE_SMS_DATA *sms_data);

#endif /* MDEVICE_H_ */
