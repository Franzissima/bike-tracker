/*
 * debug.h
 *
 *  Created on: 20.05.2014
 *      Author: andreasbehnke
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "hardware.h"
#include "uart.h"

#ifdef DEBUG

FILE *debug_stream;

#define debug_init() \
do { \
    uart_async_init(DEBUG_UART, DEBUG_BAUD, DEBUG_IN_BUF_SIZE, DEBUG_OUT_BUF_SIZE); \
    debug_stream = uart_async_open_stream(1,1); \
} while(0)

#define debug_puts(str) fputs(str, debug_stream);

#define debug_printf(str, val) fprintf(debug_stream, str, val);

#else

#define debug_init()
#define debug_puts(str)
#define debug_printf(str, val)

#endif

#endif /* DEBUG_H_ */
