/*
 * uart.c
 *
 * Implementation of the UART API for several AVR controller
 *
 *  Created on: 26.03.2014
 *      Author: andreasbehnke
 */
#include <avr/io.h>
#include <util/delay.h>
#include "include/uart.h"

// Implementation for controller with single UART
#ifdef UCSRA
void uart_init(uint8_t uart_index, uint16_t uart_baud) {
    UBRRH = (uint8_t) (uart_baud>>8);
    UBRRL = (uint8_t) (uart_baud);
	UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRC = ( 1 << URSEL )|(1 << UCSZ1)|(1 << UCSZ0); // 8 bit and 1 stop bit
}

/*
 * Sends one character to UART
 */
int uart_send_char(char c, FILE *dummy)
{
	loop_until_bit_is_set(UCSRA, UDRE); // wait until ready
    UDR = c;                            // send character
	return (0);
}

/*
 * Receive one character from UART. Waits until timeout reached.
 */
int uart_receive_char(FILE *dummy)
{
    while (!(UCSRA & (1<<RXC))) {}
	return UDR;
}

FILE *uart_open_stream(uint8_t uart_index) {
	return fdevopen (uart_send_char, uart_receive_char);
}
#endif
