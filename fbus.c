/*
 * fbus.c
 *
 *  Created on: 21.04.2014
 *      Author: andreasbehnke
 */

#include "include/fbus.h"
#include <util/delay.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <stdio.h>

#define FBUS_SEQUENCE_UPPER_BITS 0x60

uint8_t fbus_sequence = 0;

uint8_t fbus_state = FBUS_STATE_NO_FRAME;

uint16_t fbus_bytes_read = 0;

FBUS_FRAME fbus_input_frame;

FILE *fbus_stream;

void fbus_init(FILE *stream) {
    fbus_stream = stream;
    fbus_input_frame.data = (uint8_t*)malloc(FBUS_MAX_DATA_LENGTH);
}

void fbus_synchronize() {
    // synchronize phone
    for (int i = 0; i < 127; ++i) {
        fputc(0x55, fbus_stream);
        _delay_us(100);
    }
}

void fbus_input_clear() {
    fbus_state = FBUS_STATE_NO_FRAME;
    fbus_bytes_read = 0;
    fbus_input_frame.data_pos = 0;
    fbus_input_frame.data_size = 0;
    fbus_input_frame.even_checksum = 0;
    fbus_input_frame.odd_checksum = 0;
}

uint8_t fbus_read_frame() {
    if (IS_FBUS_ERROR() || IS_FBUS_READY()) {
        return fbus_state;
    }
    int input = fgetc(fbus_stream);
    if (input == EOF) {
        return FBUS_STATE_INPUT_QUEUE_EMPTY;
    }
    uint8_t c = input;
    if (fbus_state < FBUS_STATE_PADDING_BYTE_READ) {
        if ((fbus_bytes_read & 0x01) == 0) {
            // even byte
            fbus_input_frame.even_checksum ^= c;
        } else {
            // odd byte
            fbus_input_frame.odd_checksum ^= c;
        }
    }
    fbus_bytes_read++;
    switch (fbus_state) {
        case FBUS_STATE_NO_FRAME:
            if (c == FBUS_FRAME_ID) {
                fbus_state++;
            } // ignore none 0x1e bytes (phone sends 0x00 some times)
            return fbus_state;
        case FBUS_STATE_FRAME_ID_READ:
            return ++fbus_state;
        case FBUS_STATE_DEST_ADR_READ:
            return ++fbus_state;
        case FBUS_STATE_SRC_ADR_READ:
            fbus_input_frame.command = c;
            return ++fbus_state;
        case FBUS_STATE_CMD_READ:
            fbus_input_frame.data_size = (c << 8);
            return ++fbus_state;
        case FBUS_STATE_SIZE_MSB_READ:
            fbus_input_frame.data_size |= c;
            fbus_input_frame.data_pos = 0;
            return ++fbus_state;
        case FBUS_STATE_SIZE_LSB_READ:
            fbus_input_frame.data[fbus_input_frame.data_pos] = c;
            fbus_input_frame.data_pos++;
            if (fbus_input_frame.data_pos == fbus_input_frame.data_size) {
                ++fbus_state;
                if ((fbus_input_frame.data_size & 0x01) == 0) {
                    // no padding byte, even data size
                    ++fbus_state;
                }
            }
            return fbus_state;
        case FBUS_STATE_DATA_READ:
            return ++fbus_state; // skip padding byte
        case FBUS_STATE_PADDING_BYTE_READ:
            if (fbus_input_frame.even_checksum != c) {
                fbus_state = FBUS_STATE_FRAME_ERROR;
                return fbus_state;
            }
            return ++fbus_state;
        case FBUS_STATE_EVEN_CHK_READ:
            if (fbus_input_frame.odd_checksum != c) {
                fbus_state = FBUS_STATE_FRAME_ERROR;
                return fbus_state;
            }
            return ++fbus_state;
    }
    // this should never happen:
    return FBUS_STATE_FRAME_ERROR;
}

void inline fbus_reset_sequence() {
    fbus_sequence = 0;
}

void fbus_send_frame(uint8_t command, uint16_t data_size, uint8_t *data) {
    // set sequence number
    data[data_size - 1] = fbus_sequence | FBUS_SEQUENCE_UPPER_BITS;
    fbus_sequence = (fbus_sequence + 1) & 0x0F;

    // write header
    fputc(FBUS_FRAME_ID, fbus_stream);
    fputc(FBUS_PHONE_ID, fbus_stream);
    fputc(FBUS_TERMINAL_ID, fbus_stream);
    fputc(command, fbus_stream);

    // initialize checksums
    uint8_t even_checksum = FBUS_FRAME_ID ^ FBUS_TERMINAL_ID;
    uint8_t odd_checksum = FBUS_PHONE_ID ^ command;

    // write size
    uint8_t msb_size = (data_size >> 8);
    even_checksum ^= msb_size;
    uint8_t lsb_size = (data_size & 0xFF);
    odd_checksum ^= lsb_size;
    fputc(msb_size, fbus_stream);
    fputc(lsb_size, fbus_stream);

    // write data
    for(int i=0; i < data_size; i++) {
        uint8_t c = data[i];
        fputc(c, fbus_stream);
        if ((i & 0x01) == 0) {
            even_checksum ^= c;
        } else {
            odd_checksum ^= c;
        }
    }

    // write padding byte
    if ((data_size & 0x01) == 1) {
        fputc(0x00, fbus_stream);
    }

    // write checksums
    fputc(even_checksum, fbus_stream);
    fputc(odd_checksum, fbus_stream);
}
