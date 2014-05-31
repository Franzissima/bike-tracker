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

#endif /* GSM_H_ */
