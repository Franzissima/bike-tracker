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
#define MDEVICE_STATE_READY             1
#define MDEVICE_STATE_WAIT_FOR_ACK      2
#define MDEVICE_STATE_WAIT_FOR_RESPONSE 3
#define MDEVICE_STATE_RESPONSE_READY    4
#define MDEVICE_STATE_ERROR             255

#define MDEVICE_PIN_ACCEPTED            0
#define MDEVICE_PIN_SIM_CARD_NOT_READY  1
#define MDEVICE_PIN_WRONG_PIN_CODE      2
#define MDEVICE_PIN_CHANGE_OK           3
#define MDEVICE_PIN_WAIT_FOR            4
#define MDEVICE_PIN_UNKNOWN             255

extern void mdevice_init();

extern uint8_t mdevice_process(FILE *debug);

extern void mdevice_tx_get_status();

extern uint8_t mdevice_get_status();

extern void mdevice_tx_get_hdw_version();

extern uint8_t *mdevice_get_hdw_version();

extern void mdevice_rc_wait_for_network_status();

extern void mdevice_tx_get_pin_status();

/*
 Returns:
 0: not ready for PIN

 or expected code:

 1: security code (5 chars)
 2: PIN (4 chars)
 3: PIN2 (4 chars)
 4: PUK (8 chars)
 5: PUK2 (8 chars)
 */
extern uint8_t mdevice_get_pin_status();

extern void mdevice_tx_enter_pin(uint8_t pin[4]);

extern uint8_t mdevice_enter_pin_result();

#endif /* MDEVICE_H_ */
