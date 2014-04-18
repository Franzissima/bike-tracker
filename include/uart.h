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

#define UART_BAUD_SELECT(baudRate, xtalCpu) (((xtalCpu) + (baudRate)*8l)/((baudRate)*16l)-1)
#define UART_BAUD UART_BAUD_SELECT(9600, F_CPU)
#define UART_TIMEOUT_MS 60000

/*
 * Initialing UART for first use
 */
extern void uart_init(uint8_t uart_index, uint16_t uart_baud);

/*
 * Initialing UART for asynchronous use, controlled by interrupts
 */
extern void uart_async_init(uint8_t uart_index, uint16_t uart_baud);

/*
 * Open stream to UART device for usage with stdio functions
 */
extern FILE *uart_open_stream(uint8_t uart_index);

/*
 * Open stream to interrupt controlled UART device for usage with stdio functions
 */
extern FILE *uart_async_open_stream(uint8_t uart_index);

#endif /* UART_H_ */
