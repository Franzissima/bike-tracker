/*
 * main.c
 *
 *  Created on: 23.03.2014
 *      Author: andreasbehnke
 */
#include "include/main.h"
#include "include/uart.h"
#include "include/buzzer.h"
#include "include/motion_detection.h"
#include "include/fifo.h"
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#ifdef TEST_UART
int main()
{
    buzzer_init();
    uart_init(0, UART_BAUD_SELECT(9600, F_CPU));
    FILE *stream0 = uart_open_stream(0);

    char buffer[256];
    char *line;

    while (1)
    {
        fputs("Type a char:\n\r", stream0);
        int c = fgetc(stream0);
        buzzer_beep(1, 50, 0);
        fputs("\n\r", stream0);
        fputc(c, stream0);
        fputs("\n\r", stream0);
        fputs("Type a line:\n\r", stream0);
        line = fgets(buffer, 256, stream0);
        fputs(line, stream0);
        fputs("\n\r", stream0);
    }
    return (1);	// should never happen
}
#endif

#ifdef TEST_MOTION_SENSOR
int main() {
    motion_detection_init();
    while (1) {
        while(motion_detection_state() == MOTION_DETECTION_ACTIVE) {

        }
        sleep_cpu();
    }
}
#endif

#ifdef TEST_BUZZER
int main() {
      TCCR0 = (1<<CS01);
      TIMSK |= (1<<TOIE0);

      sei();

      buzzer_init();
      while(1)
      {
          if(buzzer_async_get_state() != BUZZER_STATE_STOP) {
              _delay_ms(100);
          } else {
              _delay_ms(5000);
              buzzer_async_beep(3,500,1000);
          }

      }
}

ISR (TIMER0_OVF_vect)
{
  buzzer_async_timer();
}
#endif

#ifdef TEST_FIFO

FIFO queue;

int main()
{
    uart_init(0, UART_BAUD_SELECT(9600, F_CPU));
    FILE *stream0 = uart_open_stream(0);
    uint8_t *byte = 0;

    while(1) {
        fifo_clear(&queue);
        fifo_write(&queue, 'A');
        fifo_write(&queue, 'B');
        fifo_write(&queue, 'C');
        fifo_write(&queue, 'D');
        fifo_write(&queue, 'E');

        fifo_read(&queue, byte);
        fputc(*byte, stream0);
        fifo_read(&queue, byte);
        fputc(*byte, stream0);
        fifo_read(&queue, byte);
        fputc(*byte, stream0);
        fifo_read(&queue, byte);
        fputc(*byte, stream0);
        if (fifo_read(&queue, byte) != FIFO_OK) {
            fputs("Error: FIFO is full\n\r", stream0);
        }
        fputc(*byte, stream0);

        fputs("\n\r", stream0);
        if (fifo_read(&queue, byte) == FIFO_EMPTY) {
            fputs("FIFO is empty\n\r", stream0);
        } else {
            fputs("Error: Expecting FIFO is empty\n\r", stream0);
        }
        fifo_clear(&queue);
        for (uint16_t i = 0; i < FIFO_BUFFER_SIZE; ++i) {
            if(fifo_write(&queue, 'A') != FIFO_OK) {
                fputs("Error: Expecting FIFO is OK\n\r", stream0);
            }
        }
        if(fifo_write(&queue, 0) != FIFO_FULL) {
            fputs("Error: Expecting FIFO overflow\n\r", stream0);
        } else {
            fputs("FIFO is full\n\r", stream0);
        }

        _delay_ms(2000);

    }
    return (1); // should never happen
}
#endif
