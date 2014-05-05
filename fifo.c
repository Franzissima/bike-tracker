/*
 * fifo.c
 *
 *  Created on: 15.04.2014
 *      Author: andreasbehnke
 */

#include <stdlib.h>
#include <util/atomic.h>
#include "include/fifo.h"

void fifo_init(FIFO *fifo, uint8_t size) {
    fifo->read = 0;
    fifo->write = 0;
    fifo->size = size;
    fifo->buffer = (uint8_t*)malloc(size);
}

uint8_t fifo_write(FIFO *fifo, uint8_t byte) {
    uint8_t next = ((fifo->write + 1) & fifo->size);
    if (fifo->read == next) {
        return FIFO_FULL;
    }
    fifo->buffer[fifo->write] = byte;
    fifo->write = next;
    return FIFO_OK;
}

uint8_t fifo_write_blocking(FIFO *fifo, uint8_t byte) {
    while(IS_FIFO_FULL((*fifo))) {}
    uint8_t state;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        state = fifo_write(fifo, byte);
    }
    return state;
}

uint8_t fifo_write_bytes(FIFO *fifo, uint8_t bytes[], uint16_t length) {
    for (int i = 0; i < length; ++i) {
        if(fifo_write(fifo, bytes[i]) == FIFO_FULL) {
            return i;
        }
    }
    return length;
}

uint8_t fifo_write_n_bytes(FIFO *fifo, uint8_t byte, uint16_t length) {
    for (int i = 0; i < length; ++i) {
        if(fifo_write(fifo, byte) == FIFO_FULL) {
            return i;
        }
    }
    return length;
}

uint8_t fifo_read(FIFO *fifo, uint8_t *byte) {
    if (fifo->read == fifo->write) {
        return FIFO_EMPTY;
    }
    *byte = fifo->buffer[fifo->read];
    fifo->read = (fifo->read + 1) & fifo->size;
    return FIFO_OK;
}
