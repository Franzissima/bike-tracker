/*
 * uart.c
 *
 * Implementation of the UART API for several AVR controller
 *
 *  Created on: 26.03.2014
 *      Author: andreasbehnke
 */
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "include/hardware.h"
#include "include/fifo.h"
#include "include/uart.h"

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

#ifdef UCSR1A
#define UART_COUNT 2
#elif defined UCSR0A
#define UART_COUNT 1
#endif

FIFO uart_input_queue[UART_COUNT];
FIFO uart_output_queue[UART_COUNT];

void uart_init(uint8_t uart_index, uint16_t uart_baud) {
#ifdef UCSR1A
    switch (uart_index) {
        case 0:
#endif
            UBRR0H = (uint8_t) (uart_baud>>8);
            UBRR0L = (uint8_t) (uart_baud);
            UCSR0B = (1<<RXEN0)|(1<<TXEN0);
#ifdef URSEL
            UCSR0C = ( 1 << URSEL )|(1 << UCSZ01)|(1 << UCSZ00); // 8 bit and 1 stop bit
#else
            UCSR0C = (1 << UCSZ01)|(1 << UCSZ00); // 8 bit and 1 stop bit
#endif
#ifdef UCSR1A
            break;
        case 1:
            UBRR1H = (uint8_t) (uart_baud>>8);
            UBRR1L = (uint8_t) (uart_baud);
            UCSR1B = (1<<RXEN1)|(1<<TXEN1);
            UCSR1C = (1 << UCSZ11)|(1 << UCSZ10); // 8 bit and 1 stop bit
            break;
        default:
            break;
    }
#endif
}

void uart_async_init(uint8_t uart_index, uint16_t uart_baud, uint8_t input_buffer_size, uint8_t output_buffer_size) {
    fifo_init(&uart_input_queue[uart_index], input_buffer_size);
    fifo_init(&uart_output_queue[uart_index], output_buffer_size);
    uart_init(uart_index, uart_baud);
#ifdef UCSR1A
    switch (uart_index) {
        case 0:
#endif
            UCSR0B |= (1 << RXCIE0);
#ifdef UCSR1A
            break;
        case 1:
            UCSR1B |= (1 << RXCIE1);
            break;
        default:
            break;
    }
#endif
}

inline FIFO *uart_get_async_input_fifo(uint8_t uart_index) {
    return &uart_input_queue[uart_index];
}

inline FIFO *uart_get_async_output_fifo(uint8_t uart_index) {
    return &uart_output_queue[uart_index];
}

ISR(USART0_RX_vect) {
    uint8_t byte = UDR0;
    fifo_write(&uart_input_queue[0], byte);
}

ISR(USART0_UDRE_vect) {
    uint8_t byte = 0;
    fifo_read(&uart_output_queue[0], &byte);
    UDR0 = byte;
    if (IS_FIFO_EMPTY(uart_output_queue[0])) {
        UCSR0B &= ~(1 << UDRIE0); // queue empty, disable interrupt
    }
}

#ifdef UCSR1A
ISR(USART1_RX_vect) {
    uint8_t byte = UDR1;
    fifo_write(&uart_input_queue[1], byte);
}

ISR(USART1_UDRE_vect) {
    uint8_t byte = 0;
    fifo_read(&uart_output_queue[1], &byte);
    UDR1 = byte;
    if (IS_FIFO_EMPTY(uart_output_queue[1])) {
        UCSR1B &= ~(1 << UDRIE1); // queue empty, disable interrupt
    }
}
#endif

/*
 * Sends one character to UART
 */
int _uart_put(char c, FILE *dummy)
{
#ifdef UCSR1A
    uint8_t *uart_index = (uint8_t*)dummy->udata;
    switch(*uart_index) {
        case 0:
#endif
            loop_until_bit_is_set(UCSR0A, UDRE0);
            UDR0 = c;
#ifdef UCSR1A
            break;
        case 1:
            loop_until_bit_is_set(UCSR1A, UDRE1);
            UDR1 = c;
            break;
    }
#endif
	return 0;
}

/*
 * Receive one character from UART.
 */
int _uart_get(FILE *dummy)
{
#ifdef UCSR1A
    uint8_t *uart_index = (uint8_t*)dummy->udata;
    switch(*uart_index) {
        case 0:
#endif
            while (!(UCSR0A & (1<<RXC0))) {}
            return UDR0;
#ifdef UCSR1A
        case 1:
            while (!(UCSR1A & (1<<RXC1))) {}
            return UDR1;
    }
    return EOF;
#endif
}

FILE *uart_open_stream(uint8_t uart_index) {
	FILE *stream = fdevopen (_uart_put, _uart_get);
	stream->udata = malloc(sizeof(uint8_t));
    *(uint8_t*)stream->udata = uart_index;
    return stream;
}

int _uart_async_get(FILE *dummy)
{
    uint8_t *uart_index = (uint8_t*)dummy->udata;
    uint8_t byte = 0;
    FIFO *queue = &uart_input_queue[*uart_index];
    if(IS_FIFO_EMPTY((*queue))) {
        return EOF;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        fifo_read(queue, &byte);
    }
    return byte;
}

int _uart_async_put(char c, FILE *dummy)
{
    uint8_t *uart_index = (uint8_t*)dummy->udata;
    FIFO *queue = &uart_output_queue[*uart_index];
    while(IS_FIFO_FULL((*queue))) {
        // warning: we should think about timer here!
        // If queue is not empty after some amount of
        // UART cycles, we should fail with EOF
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if(IS_FIFO_EMPTY((*queue))) {
            // queue is empty, enable data ready interrupt
#ifdef UCSR1A
            switch (*uart_index) {
                case 0:
#endif
                    UCSR0B |= (1 << UDRIE0);
                    UDR0 = c;
#ifdef UCSR1A
                    break;
                case 1:
                    UCSR1B |= (1 << UDRIE1);
                    UDR1 = c;
                    break;
                default:
                    break;
            }
#endif
        } else {
            fifo_write(queue, c);
        }
    }
    return 0;
}

FILE *uart_async_open_stream(uint8_t uart_index) {
    FILE *stream = fdevopen (_uart_async_put, _uart_async_get);
    stream->udata = malloc(sizeof(uint8_t));
    *(uint8_t*)stream->udata = uart_index;
    return stream;
}
