/*
 * gsm.h
 *
 *  Created on: 31.05.2014
 *      Author: andreasbehnke
 */

#ifndef GSM_H_
#define GSM_H_

#include <inttypes.h>

extern uint8_t gsm_pack_7bit(uint8_t *dest, uint8_t *src, uint8_t src_length);

extern uint8_t gsm_unpack_7bit(uint8_t *encoded, uint8_t *input, uint8_t in_length);

#endif /* GSM_H_ */
