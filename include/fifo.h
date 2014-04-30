/*
 * fifo.h
 *
 *  Created on: 15.04.2014
 *      Author: andreasbehnke
 */

#ifndef FIFO_H_
#define FIFO_H_

#include <inttypes.h>

#define FIFO_OK 0
#define FIFO_FULL 1
#define FIFO_EMPTY 2
#define IS_FIFO_EMPTY(queue) queue.read == queue.write
#define IS_FIFO_FULL(queue) queue.read == ((queue.write + 1) & queue.size)
#define IS_FIFO_EMPTY_P(queue) queue->read == queue->write
#define IS_FIFO_FULL_P(queue) queue->read == ((queue->write + 1) & queue->size)

typedef struct {
  volatile uint8_t read;
  volatile uint8_t write;
  uint8_t size;
  uint8_t *buffer;
} FIFO;

extern void fifo_init(FIFO *fifo, uint8_t size);

extern uint8_t fifo_write(FIFO *fifo, uint8_t byte);

extern uint8_t fifo_write_blocking(FIFO *fifo, uint8_t byte);

extern uint8_t fifo_write_bytes(FIFO *fifo, uint8_t bytes[], uint16_t length);

extern uint8_t fifo_write_n_bytes(FIFO *fifo, uint8_t byte, uint16_t length);

extern uint8_t fifo_read(FIFO *fifo, uint8_t *byte);

#endif /* FIFO_H_ */
