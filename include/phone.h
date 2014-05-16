/*
 * phone.h
 *
 *  Created on: 03.05.2014
 *      Author: andreasbehnke
 */

#ifndef PHONE_H_
#define PHONE_H_

#include <stdio.h>

#include <inttypes.h>
#include "hardware.h"

#define PHONE_STATE_OFF               0
#define PHONE_STATE_READY             1
#define PHONE_STATE_WAIT_FOR_ACK      2
#define PHONE_STATE_WAIT_FOR_RESPONSE 3
#define PHONE_STATE_RESPONSE_READY    4
#define PHONE_STATE_ERROR             255

extern volatile uint8_t phone_state;

extern void phone_init();

extern uint8_t phone_process(FILE *debug);

extern void phone_tx_enable_extended_cmd();

extern void phone_tx_get_hdw_version();

#endif /* PHONE_H_ */
