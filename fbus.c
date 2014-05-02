/*
 * fbus.c
 *
 *  Created on: 21.04.2014
 *      Author: andreasbehnke
 */

#include "include/fbus.h"
#include <util/atomic.h>
#include <stdlib.h>

volatile uint8_t fbus_state = FBUS_STATE_NO_FRAME;

uint16_t fbus_bytes_read = 0;

FBUS_FRAME fbus_input_frame;

FIFO *fbus_input;

FIFO *fbus_output;

void fbus_init(FIFO *output, FIFO *input) {
    fbus_input = input;
    fbus_output = output;
    fbus_input_frame.data = (uint8_t*)malloc(FBUS_MAX_DATA_LENGTH);
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

inline uint8_t _fbus_expect_value(uint8_t actual, uint8_t expected) {
    if (expected == actual) {
        fbus_state++;
    } else {
        fbus_state = FBUS_STATE_FRAME_ERROR;
    }
    return fbus_state;
}

uint8_t fbus_read_frame() {
    if (IS_FBUS_ERROR() || IS_FBUS_READY()) {
        return fbus_state;
    }
    uint8_t c = 0;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        if (IS_FIFO_EMPTY((*fbus_input))) {
            return FBUS_STATE_INPUT_QUEUE_EMPTY;
        }
        fifo_read(fbus_input, &c);
    }
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
            return _fbus_expect_value(c, FBUS_FRAME_ID);
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

void fbus_async_timer() {
    if (IS_FBUS_ERROR() || IS_FBUS_READY()) {
        return;
    }
    // process input queue
    uint8_t state;
    do {
        state = fbus_read_frame();
    } while (state != FBUS_STATE_INPUT_QUEUE_EMPTY && !IS_FBUS_READY() && !IS_FBUS_ERROR());
}

void fbus_send_frame(uint8_t command, uint16_t data_size, uint8_t *data) {
    // write headerdata_size
    fifo_write_blocking(fbus_output, FBUS_FRAME_ID);
    fifo_write_blocking(fbus_output, FBUS_PHONE_ID);
    fifo_write_blocking(fbus_output, FBUS_TERMINAL_ID);
    fifo_write_blocking(fbus_output, command);

    // initialize checksums
    uint8_t even_checksum = FBUS_FRAME_ID ^ FBUS_TERMINAL_ID;
    uint8_t odd_checksum = FBUS_PHONE_ID ^ command;

    // write size
    uint8_t msb_size = (data_size >> 8);
    even_checksum ^= msb_size;
    uint8_t lsb_size = (data_size & 0xFF);
    odd_checksum ^= lsb_size;
    fifo_write_blocking(fbus_output, msb_size);
    fifo_write_blocking(fbus_output, lsb_size);

    // write data
    for(int i=0; i < data_size; i++) {
        uint8_t c = data[i];
        fifo_write_blocking(fbus_output, c);
        if ((i & 0x01) == 0) {
            even_checksum ^= c;
        } else {
            odd_checksum ^= c;
        }
    }

    // write padding byte
    if ((data_size & 0x01) == 1) {
        fifo_write_blocking(fbus_output, 0x00);
    }

    // write checksums
    fifo_write_blocking(fbus_output, even_checksum);
    fifo_write_blocking(fbus_output, odd_checksum);
}
