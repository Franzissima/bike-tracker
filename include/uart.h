/*
 * uart.h
 *
 *  Created on: 26.03.2014
 *      Author: andreasbehnke
 */

#ifndef UART_H_
#define UART_H_

#include <stdio.h>
#include <ctype.h>
#include "fifo.h"

#define UART_BAUD_SELECT(baudRate, xtalCpu) (((xtalCpu) + (baudRate)*8l)/((baudRate)*16l)-1)

/*
 * Initialing UART for first use
 */
extern void uart_init(uint8_t uart_index, uint16_t uart_baud);

/*
 * Initialing UART for asynchronous use, controlled by interrupts
 */
extern void uart_async_init(uint8_t uart_index, uint16_t uart_baud, uint8_t input_buffer_size, uint8_t output_buffer_size);

/*
 * Get the fifo used for asynchronous data receiving
 */
extern FIFO *uart_get_async_input_fifo(uint8_t uart_index);

/*
 * Get the fifo used for asynchronous data transmission
 */
extern FIFO *uart_get_async_output_fifo(uint8_t uart_index);

/*
 * Open stream to UART device for usage with stdio functions
 */
extern FILE *uart_open_stream(uint8_t uart_index);

/*
 * Open stream to interrupt controlled UART device for usage with stdio functions
 */
extern FILE *uart_async_open_stream(uint8_t uart_index, uint8_t wait_for_input);

#endif /* UART_H_ */
