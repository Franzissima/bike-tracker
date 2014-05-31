/*
 * gsm.c
 *
 *  Created on: 31.05.2014
 *      Author: andreasbehnke
 */

#include "include/gsm.h"

uint8_t gsm_pack_7bit(uint8_t *encode, uint8_t *input, uint8_t len) {
    uint8_t c, w;
    uint8_t n, shift = 0, x = 0;
    for (n = 0; n < len; ++n) {
        c = input[n] & 0x7f;
        c >>= shift;
        w = input[n + 1] & 0x7f;
        w <<= (7 - shift);
        shift += 1;
        c = c | w;
        if (shift == 7) {
            shift = 0x00;
            n++;
        }
        encode[x++] = c;
    }
    return x;
}
