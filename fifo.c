/*
 * fifo.c
 *
 *  Created on: 15.04.2014
 *      Author: andreasbehnke
 */

#include <stdlib.h>
#include <util/atomic.h>
#include "include/fifo.h"

typedef struct {
    FIFO *input;
    FIFO *output;
} _FIFO_IO;

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

int _fifo_get(FILE *stream)
{
    _FIFO_IO *io = (_FIFO_IO*)stream->udata;
    uint8_t byte = 0;
    if (fifo_read(io->input, &byte) == FIFO_EMPTY) {
        return EOF;
    }
    return byte;
}

int _fifo_put(char c, FILE *stream) {
    _FIFO_IO *io = (_FIFO_IO*)stream->udata;
    FIFO output = (*io->output);
    if (IS_FIFO_FULL(output)) {
        return EOF;
    }
    fifo_write(&output, c);
    return 0;
}

FILE *fifo_open_stream(FIFO *input, FIFO *output) {
    _FIFO_IO io = {input, output};
    FILE *stream = fdevopen (_fifo_put, _fifo_get);
    stream->udata = malloc(sizeof(_FIFO_IO));
    *(_FIFO_IO*)stream->udata = io;
    return stream;
}
