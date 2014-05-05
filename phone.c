/*
 * phone.c
 *
 *  Created on: 03.05.2014
 *      Author: andreasbehnke
 */

#include <avr/interrupt.h>
#include "include/phone.h"
#include "include/uart.h"
#include "include/fbus.h"

volatile uint8_t phone_state = PHONE_STATE_OFF;

FIFO *phone_in;

FIFO *phone_out;

void phone_init() {
    // initialize UART
    uart_async_init(PHONE_UART, PHONE_BAUD, PHONE_IN_BUF_SIZE, PHONE_OUT_BUF_SIZE);
    FIFO *phone_in = uart_get_async_input_fifo(PHONE_UART);
	FIFO *phone_out = uart_get_async_output_fifo(PHONE_UART);
	fbus_init(phone_out, phone_in);
	fbus_input_clear();

	// TODO: turn phone on
}

uint8_t phone_process(FILE *debug) {
    uint8_t fbus_state = fbus_read_frame();
    if (fbus_state == FBUS_STATE_INPUT_QUEUE_EMPTY) {
        return phone_state;
    }
    switch (phone_state) {
        case PHONE_STATE_OFF:
            if (IS_FBUS_ERROR()) {
                fputs("FBUS ERROR", debug);
                fbus_input_clear();
            }
            if (IS_FBUS_READY()) {
                fputs("FBUS READY", debug);
                /*
                    Nokia 3310 startup commands:

                    1E FF 00 D0 00 03 01 01 E0 00 FF 2D  First Command
                    1E 14 00 F4 00 01 03 00 1D E1        Second Command
                 */

                uint8_t command = fbus_input_frame.command;
                fbus_input_clear();
                if (command == 0xF4) {
                    // Receiving of these two frames is no indicator for end of power-on-pulse!
                    phone_state = PHONE_STATE_ON;
                }
            }

            break;
        default:
            break;
    }
    return phone_state;
}
