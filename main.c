/*
 * main.c
 *
 *  Created on: 23.03.2014
 *      Author: andreasbehnke
 */
#include <util/delay.h>
#include <stdlib.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "include/main.h"
#include "include/uart.h"
#include "include/buzzer.h"
#include "include/motion_detection.h"
#include "include/fifo.h"
#include "include/fbus.h"

#ifdef TEST_UART
int main()
{
    buzzer_init();
    uart_async_init(0, UART_BAUD_SELECT(9600, F_CPU), 3, 7);
    FILE *stream0 = uart_async_open_stream(0);

    char buffer[256];
    char *line;

    sei();

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

#ifdef TEST_FBUS
FIFO input;
FILE *output;

void assertEqualsUint8(uint8_t expected, uint8_t actual, const char* message) {
    if (expected != actual) {
        fputs(message, output);
        fputs("\n\r", output);
        fputs("But got:", output);
        char buffer[] = "123456";
        itoa(actual, buffer, 10);
        fputs(buffer, output);
        fputs("\n\r", output);
    }
}

void assertEqualsUint16(uint16_t expected, uint16_t actual, const char* message) {
    if (expected != actual) {
        fputs(message, output);
        fputs("\n\r", output);
    }
}

int main()
{
    uart_init(0, UART_BAUD_SELECT(9600, F_CPU));
    output = uart_open_stream(0);
    fputs("start tests\n\r", output);

    fifo_init(&input, 255);

    // test even data size

    uint8_t test_frame1[] = {0x1E, 0x0D, 0x00, 0x7F, 0x00, 0x02, 0xD2, 0x01, 0xCC, 0x71};
    fifo_write_bytes(&input, test_frame1, 10);
    fbus_input_clear();
    assertEqualsUint8(FBUS_STATE_NO_FRAME, fbus_state, "Expected FBUS_STATE_NO_FRAME");
    assertEqualsUint8(FBUS_STATE_FRAME_ID_READ, fbus_read_frame(&input), "Expected FBUS_STATE_FRAME_ID_READ");
    assertEqualsUint8(FBUS_STATE_DEST_ADR_READ, fbus_read_frame(&input), "Expected FBUS_STATE_DEST_ADR_READ");
    assertEqualsUint8(FBUS_STATE_SRC_ADR_READ, fbus_read_frame(&input), "Expected FBUS_STATE_SRC_ADR_READ");
    assertEqualsUint8(FBUS_STATE_CMD_READ, fbus_read_frame(&input), "Expected FBUS_STATE_CMD_READ");
    assertEqualsUint8(0x7F, fbus_input_frame.command, "Expected 0x7F");
    assertEqualsUint8(FBUS_STATE_SIZE_MSB_READ, fbus_read_frame(&input), "Expected FBUS_STATE_SIZE_MSB_READ");
    assertEqualsUint16(0x0000, fbus_input_frame.data_size, "Expected 0x0000");
    assertEqualsUint8(FBUS_STATE_SIZE_LSB_READ, fbus_read_frame(&input), "Expected FBUS_STATE_SIZE_LSB_READ");
    assertEqualsUint16(0x0002, fbus_input_frame.data_size, "Expected 0x0002");
    assertEqualsUint8(FBUS_STATE_SIZE_LSB_READ, fbus_read_frame(&input), "Expected FBUS_STATE_SIZE_LSB_READ");
    assertEqualsUint8(0xD2, fbus_input_frame.data[0], "Expected 0xD2");
    assertEqualsUint8(FBUS_STATE_PADDING_BYTE_READ, fbus_read_frame(&input), "Expected FBUS_STATE_PADDING_BYTE_READ");
    assertEqualsUint8(0x01, fbus_input_frame.data[1], "Expected 0x01");
    assertEqualsUint8(FBUS_STATE_EVEN_CHK_READ, fbus_read_frame(&input), "Expected FBUS_STATE_EVEN_CHK_READ");
    assertEqualsUint8(FBUS_STATE_FRAME_READY, fbus_read_frame(&input), "Expected FBUS_STATE_FRAME_READY");
    assertEqualsUint8(0xCC, fbus_input_frame.even_checksum, "Expected 0xCC");
    assertEqualsUint8(0x71, fbus_input_frame.odd_checksum, "Expected 0x71");

    fputs("Finished test even data size\n\r", output);

    // test odd data size

    uint8_t test_frame2[] = {0x1E, 0x0D, 0x00, 0x7F, 0x00, 0x03, 0xD2, 0x01, 0x10, 0x00, 0xDC, 0x70};
    fifo_write_bytes(&input, test_frame2, 12);
    fbus_input_clear();
    assertEqualsUint8(FBUS_STATE_FRAME_ID_READ, fbus_read_frame(&input), "Expected FBUS_STATE_FRAME_ID_READ");
    assertEqualsUint8(FBUS_STATE_DEST_ADR_READ, fbus_read_frame(&input), "Expected FBUS_STATE_DEST_ADR_READ");
    assertEqualsUint8(FBUS_STATE_SRC_ADR_READ, fbus_read_frame(&input), "Expected FBUS_STATE_SRC_ADR_READ");
    assertEqualsUint8(FBUS_STATE_CMD_READ, fbus_read_frame(&input), "Expected FBUS_STATE_CMD_READ");
    assertEqualsUint8(FBUS_STATE_SIZE_MSB_READ, fbus_read_frame(&input), "Expected FBUS_STATE_SIZE_MSB_READ");
    assertEqualsUint16(0x0000, fbus_input_frame.data_size, "Expected 0x0000");
    assertEqualsUint8(FBUS_STATE_SIZE_LSB_READ, fbus_read_frame(&input), "Expected FBUS_STATE_SIZE_LSB_READ");
    assertEqualsUint16(0x0003, fbus_input_frame.data_size, "Expected 0x0002");
    assertEqualsUint8(FBUS_STATE_SIZE_LSB_READ, fbus_read_frame(&input), "Expected FBUS_STATE_SIZE_LSB_READ");
    assertEqualsUint8(0xD2, fbus_input_frame.data[0], "Expected 0xD2");
    assertEqualsUint8(FBUS_STATE_SIZE_LSB_READ, fbus_read_frame(&input), "Expected FBUS_STATE_SIZE_LSB_READ");
    assertEqualsUint8(0x01, fbus_input_frame.data[1], "Expected 0x01");
    assertEqualsUint8(FBUS_STATE_DATA_READ, fbus_read_frame(&input), "Expected FBUS_STATE_DATA_READ");
    assertEqualsUint8(0x10, fbus_input_frame.data[2], "Expected 0x10");
    assertEqualsUint8(FBUS_STATE_PADDING_BYTE_READ, fbus_read_frame(&input), "Expected FBUS_STATE_PADDING_BYTE_READ");
    assertEqualsUint8(FBUS_STATE_EVEN_CHK_READ, fbus_read_frame(&input), "Expected FBUS_STATE_EVEN_CHK_READ");
    assertEqualsUint8(FBUS_STATE_FRAME_READY, fbus_read_frame(&input), "Expected FBUS_STATE_FRAME_READY");
    assertEqualsUint8(0xDC, fbus_input_frame.even_checksum, "Expected 0xDC");
    assertEqualsUint8(0x71, fbus_input_frame.odd_checksum, "Expected 0x71");

    fputs("Finished test odd data size\n\r", output);

    while(1) {

    }
    return (1); // should never happen
}

#endif
