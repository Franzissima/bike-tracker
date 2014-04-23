/*
 * fbus.c
 *
 *  Created on: 21.04.2014
 *      Author: andreasbehnke
 */

#include "include/fbus.h"
#include <util/atomic.h>

uint8_t fbus_state = FBUS_STATE_NO_FRAME;

uint16_t fbus_bytes_read = 0;

FBUS_FRAME fbus_input_frame;

inline uint8_t fbus_expect_value(uint8_t actual, uint8_t expected) {
    if (expected == actual) {
        fbus_state++;
    } else {
        fbus_state = FBUS_STATE_FRAME_ERROR;
    }
    return fbus_state;
}

void fbus_input_clear() {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        fbus_state = FBUS_STATE_NO_FRAME;
        fbus_bytes_read = 0;
        fbus_input_frame.data_pos = 0;
        fbus_input_frame.data_size = 0;
        fbus_input_frame.even_checksum = 0;
        fbus_input_frame.odd_checksum = 0;
    }
}

uint8_t fbus_read_frame(FIFO *input) {
    uint8_t c = 0;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        if (input->read == input->write
                || fbus_state == FBUS_STATE_FRAME_ERROR
                || fbus_state == FBUS_STATE_FRAME_READY) {
            return fbus_state;
        }
        fifo_read(input, &c);
    }
    if ((fbus_bytes_read & 0x01) == 0) {
        // even byte
        fbus_input_frame.even_checksum ^= c;
    } else if (fbus_state != FBUS_STATE_PADDING_BYTE_READ) { // do not check even checksum
        // odd byte
        fbus_input_frame.odd_checksum ^= c;
    }
    fbus_bytes_read++;
    switch (fbus_state) {
        case FBUS_STATE_NO_FRAME:
            return fbus_expect_value(c, FBUS_FRAME_ID);
        case FBUS_STATE_FRAME_ID_READ:
            return fbus_expect_value(c, FBUS_TERMINAL_ID);
        case FBUS_STATE_DEST_ADR_READ:
            return fbus_expect_value(c, FBUS_PHONE_ID);
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
                if ((fbus_input_frame.data_size & 1) == 0) {
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
