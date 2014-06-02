/*
 * gsm.c
 *
 *  Created on: 31.05.2014
 *      Author: andreasbehnke
 */

/**
 * Code inspired by gsm-encoding.c, gnooki project www.gnokii.org
 */

#include "include/gsm.h"

uint8_t gsm_pack_7bit(uint8_t *encoded, uint8_t *input, uint8_t in_length) {
    uint8_t c, w;
    uint8_t n, shift = 0, x = 0;
    for (n = 0; n < in_length; ++n) {
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
        encoded[x++] = c;
    }
    return x;
}

uint8_t gsm_unpack_7bit(uint8_t *output, uint8_t *encoded, uint8_t in_length)
{
    uint8_t *out_num = output; /* Current pointer to the output buffer */
    uint8_t *in_num = encoded;  /* Current pointer to the input buffer */
    uint8_t rest = 0x00;
    int bits = 7;

    while ((in_num - encoded) < in_length) {

        *out_num = ((*in_num & ((1 << bits) - 1)) << (7 - bits)) | rest;
        rest = *in_num >> bits;

        /* If we don't start from 0th bit, we shouldn't go to the
           next char. Under *out_num we have now 0 and under Rest -
           _first_ part of the char. */
        if ((in_num != encoded) || (bits == 7)) out_num++;
        in_num++;

        /* After reading 7 octets we have read 7 full characters but
           we have 7 bits as well. This is the next character */
        if (bits == 1) {
            *out_num = rest;
            out_num++;
            bits = 7;
            rest = 0x00;
        } else {
            bits--;
        }
    }

    return out_num - output;
}
