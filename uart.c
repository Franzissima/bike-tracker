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

#ifdef UCSRA
#define UBRR0H UBRRH
#define UBRR0L UBRRL
#define UCSR0A UCSRA
#define UCSR0B UCSRB
#define UCSR0C UCSRC
#define RXEN0  RXEN
#define TXEN0  TXEN
#define UCSZ00 UCSZ0
#define UCSZ01 UCSZ1
#define RXCIE0 RXCIE
#define RXC0   RXC
#define UDR0   UDR
#define UDRIE0 UDRIE
#define UDRE0  UDRE
#define USART0_RX_vect   USART_RXC_vect
#define USART0_UDRE_vect USART_UDRE_vect
#endif

FIFO uart_input_queue0;
FIFO uart_output_queue0;

void uart_init(uint8_t uart_index, uint16_t uart_baud) {
    UBRR0H = (uint8_t) (uart_baud>>8);
    UBRR0L = (uint8_t) (uart_baud);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
#ifdef URSEL
    UCSR0C = ( 1 << URSEL )|(1 << UCSZ01)|(1 << UCSZ00); // 8 bit and 1 stop bit
#else
	UCSR0C = (1 << UCSZ01)|(1 << UCSZ00); // 8 bit and 1 stop bit
#endif
}

void uart_async_init(uint8_t uart_index, uint16_t uart_baud, uint8_t input_buffer_size, uint8_t output_buffer_size) {
    fifo_init(&uart_input_queue0, input_buffer_size);
    fifo_init(&uart_output_queue0, output_buffer_size);
    uart_init(uart_index, uart_baud);
    UCSR0B |= (1 << RXCIE0);
}

FIFO *uart_get_async_input(uint8_t uart_index) {
    return &uart_input_queue0;
}

ISR(USART0_RX_vect) {
    uint8_t byte = UDR0;
    fifo_write(&uart_input_queue0, byte);
}

ISR(USART0_UDRE_vect) {
    uint8_t byte = 0;
    fifo_read(&uart_output_queue0, &byte);
    UDR0 = byte;
    if (IS_FIFO_EMPTY(uart_output_queue0)) {
        UCSR0B &= ~(1 << UDRIE0); // queue empty, disable interrupt
    }
}

/*
 * Sends one character to UART
 */
int uart_send_char(char c, FILE *dummy)
{
	loop_until_bit_is_set(UCSR0A, UDRE0); // wait until ready
	UDR0 = c;                            // send character
	return 0;
}

/*
 * Receive one character from UART.
 */
int uart_receive_char(FILE *dummy)
{
    while (!(UCSR0A & (1<<RXC0))) {}
	return UDR0;
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
            UCSR0B |= (1 << UDRIE0);
        }
        fifo_write(&uart_output_queue0, c);
    }
    return 0;
}

FILE *uart_async_open_stream(uint8_t uart_index) {
    return fdevopen (uart_async_send_char, uart_async_receive_char);
}
