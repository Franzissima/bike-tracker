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

// implementation for controller with multiple UART's
#ifdef UCSR1A

FILE *uart_stream_0;
FILE *uart_stream_1;

void uart_init(uint8_t uart_index, uint16_t uart_baud) {
    switch (uart_index) {
        case 0:
            UCSR0B = (1<<RXEN0)|(1<<TXEN0);
            UCSR0C = ( 1 << UMSEL00 )|(1 << UCSZ01)|(1 << UCSZ00);
            UBRR0H = (uint8_t) (uart_baud>>8);
            UBRR0L = (uint8_t) (uart_baud);
            break;
        case 1:
            UCSR1B = (1<<RXEN1)|(1<<TXEN1);
            UCSR1C = ( 1 << UMSEL10 )|(1 << UCSZ11)|(1 << UCSZ10);
            UBRR1H = (uint8_t) (uart_baud>>8);
            UBRR1L = (uint8_t) (uart_baud);
            break;
        default:
            break;
    }
}

int uart_get_index(FILE *stream) {
    if (uart_stream_0 == stream) {
        return 0;
    }
    if (uart_stream_1 == stream) {
        return 1;
    }
    return -1;
}

/*
 * Sends one character to UART
 */
int uart_send_char(char c, FILE *stream)
{
    int index = uart_get_index(stream);
    switch(index) {
        case 0:
            loop_until_bit_is_set(UCSR0A, UDRE0);
            UDR0 = c;
            break;
        case 1:
            loop_until_bit_is_set(UCSR1A, UDRE1);
            UDR1 = c;
            break;
    }
	return (0);
}

/*
 * Receive one character from UART. Waits until timeout reached.
 */
int uart_receive_char(FILE *stream)
{
	int timer = 0;
	int index = uart_get_index(stream);
	switch (index) {
        case 0:
            while ( !(UCSR0A & (1<<RXC0)) ) {
                _delay_ms(1);
                timer++;
                if (timer > UART_TIMEOUT_MS) {
                    return EOF;
                }
            }
            return UDR0;
            break;
        default:
            while ( !(UCSR1A & (1<<RXC1)) ) {
                _delay_ms(1);
                timer++;
                if (timer > UART_TIMEOUT_MS) {
                    return EOF;
                }
            }
            return UDR1;
            break;
    }
	return EOF;
}

FILE *uart_open_stream(uint8_t uart_index) {
    FILE *stream = fdevopen (uart_send_char, uart_receive_char);
    switch(uart_index) {
        case 0:
            uart_stream_0 = stream;
            break;
        case 1:
            uart_stream_1 = stream;
            break;
    }
	return stream;
}
#endif
