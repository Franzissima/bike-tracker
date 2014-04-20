/*
 * fifo.h
 *
 *  Created on: 15.04.2014
 *      Author: andreasbehnke
 */

#ifndef FIFO_H_
#define FIFO_H_

#include <inttypes.h>

#define FIFO_SIZE 63
#define FIFO_BUFFER_MASK FIFO_SIZE
#define FIFO_OK 0
#define FIFO_FULL 1
#define FIFO_EMPTY 2
#define IS_FIFO_EMPTY(queue) queue.read == queue.write
#define IS_FIFO_FULL(queue) queue.read == ((queue.write + 1) & FIFO_BUFFER_MASK)

typedef struct {
  volatile uint8_t read;
  volatile uint8_t write;
  uint8_t buffer[(FIFO_SIZE + 1)];
} FIFO;

extern void fifo_clear(FIFO *fifo);

extern uint8_t fifo_write(FIFO *fifo, uint8_t byte);

extern uint8_t fifo_read(FIFO *fifo, uint8_t *byte);

#endif /* FIFO_H_ */
