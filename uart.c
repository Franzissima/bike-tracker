/*
 * uart.c
 *
 * Implementation of the UART API for several AVR controller
 *
 *  Created on: 26.03.2014
 *      Author: andreasbehnke
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "include/fifo.h"
#include "include/uart.h"
#include "include/buzzer.h"

// Implementation for controller with single UART
#ifdef UCSRA

FIFO uart_input_queue0;
FIFO uart_output_queue0;

void uart_init(uint8_t uart_index, uint16_t uart_baud) {
    UBRRH = (uint8_t) (uart_baud>>8);
    UBRRL = (uint8_t) (uart_baud);
	UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRC = ( 1 << URSEL )|(1 << UCSZ1)|(1 << UCSZ0); // 8 bit and 1 stop bit
}

void uart_async_init(uint8_t uart_index, uint16_t uart_baud, uint8_t input_buffer_size, uint8_t output_buffer_size) {
    fifo_init(&uart_input_queue0, input_buffer_size);
    fifo_init(&uart_output_queue0, output_buffer_size);
    uart_init(uart_index, uart_baud);
    UCSRB |= (1 << RXCIE );
}

ISR(USART_RXC_vect) {
    uint8_t byte = UDR;
    fifo_write(&uart_input_queue0, byte);
}

ISR(USART_UDRE_vect) {
    uint8_t byte = 0;
    fifo_read(&uart_output_queue0, &byte);
    UDR = byte;
    if (IS_FIFO_EMPTY(uart_output_queue0)) {
        UCSRB &= ~(1 << UDRIE); // queue empty, disable interrupt
    }
}

/*
 * Sends one character to UART
 */
int uart_send_char(char c, FILE *dummy)
{
	loop_until_bit_is_set(UCSRA, UDRE); // wait until ready
    UDR = c;                            // send character
	return 0;
}

/*
 * Receive one character from UART.
 */
int uart_receive_char(FILE *dummy)
{
    while (!(UCSRA & (1<<RXC))) {}
	return UDR;
}

FILE *uart_open_stream(uint8_t uart_index) {
	return fdevopen (uart_send_char, uart_receive_char);
}

int uart_async_receive_char(FILE *dummy)
{
    uint8_t byte = 0;
    while(IS_FIFO_EMPTY(uart_input_queue0)) {}
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        fifo_read(&uart_input_queue0, &byte);
    }
    return byte;
}

int uart_async_send_char(char c, FILE *dummy)
{
    while(IS_FIFO_FULL(uart_output_queue0)) {}
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        if(IS_FIFO_EMPTY(uart_output_queue0)) {
            // queue is empty, enable data ready interrupt
            UCSRB |= (1 << UDRIE);
        }
        fifo_write(&uart_output_queue0, c);
    }
    return 0;
}

FILE *uart_async_open_stream(uint8_t uart_index) {
    return fdevopen (uart_async_send_char, uart_async_receive_char);
}
#endif
