/*
 * main.c
 *
 *  Created on: 23.03.2014
 *      Author: andreasbehnke
 */
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/sleep.h>
#include <string.h>
#include "include/main.h"
#include "include/timer.h"
#include "include/buzzer.h"
#include "include/uart.h"
#include "include/motion_detection.h"
#include "include/fifo.h"
#include "include/fbus.h"
#include "include/led.h"
#include "include/mdevice.h"
#include "include/mobile.h"
#include "include/debug.h"
#include "include/gsm.h"

#ifdef TEST_UART
int main()
{
    timer_init();
    buzzer_init();
    uart_init(1, UART_BAUD_SELECT(9600, F_CPU));

    FILE *stream = uart_open_stream(1);

    char buffer[256];
    char *line;

    sei();

    fputs("*** Test UART without interrupt:\n\r", stream);
    fputs("Type a char:\n\r", stream);
    int c = fgetc(stream);
    buzzer_beep(1, 50, 0);
    fputs("\n\r", stream);
    fputc(c, stream);
    fputs("\n\r", stream);
    fputs("Type a line:\n\r", stream);
    line = fgets(buffer, 256, stream);
    fputs(line, stream);
    fputs("\n\r", stream);
    fputs("Testing timeout, do NOT press any key.\n\r", stream);
    c = fgetc(stream);
    if (c == EOF) {
        fputs("Timeout reached, got EOF\n\r", stream);
    } else {
        fputs("Error: Timeout not reached, got unexpected input\n\r", stream);
    }

    cli();
    uart_async_init(1, UART_BAUD_SELECT(9600, F_CPU), 3, 7);
        FILE *streamAsync = uart_async_open_stream(1, 1);
    sei();

    fputs("*** Test async stream:\n\r", streamAsync);
    fputs("Type a char:\n\r", streamAsync);
    c = fgetc(streamAsync);
    buzzer_beep(1, 50, 0);
    fputs("\n\r", streamAsync);
    fputc(c, streamAsync);
    fputs("\n\r", streamAsync);
    fputs("Type a line:\n\r", streamAsync);
    line = fgets(buffer, 256, streamAsync);
    fputs(line, streamAsync);
    fputs("\n\r", streamAsync);
    fputs("Testing timeout, do NOT press any key.\n\r", streamAsync);
    c = fgetc(streamAsync);
    if (c == EOF) {
        fputs("Timeout reached, got EOF\n\r", streamAsync);
    } else {
        fputs("Error: Timeout not reached, got unexpected input\n\r", streamAsync);
    }

    while (1) {}
}
#endif

#ifdef TEST_UART_TO_UART
int main()
{
    buzzer_init();
    uart_async_init(0, UART_BAUD_SELECT(115200, F_CPU), 31, 31);
    FILE *stream0 = uart_async_open_stream(0);
    uart_async_init(1, UART_BAUD_SELECT(115200, F_CPU), 255, 255);
    FILE *stream1 = uart_async_open_stream(1);

    sei();

    while (1)
    {
        fputs("Type a char:\n\r", stream0);
        int c = fgetc(stream0);
        buzzer_beep(1, 50, 0);
        fputc(c, stream1);
    }
    return (1); // should never happen
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

    debug_init();
    led_init();
    timer_init();
    buzzer_init();

    sei();

    buzzer_beep(3,100,500);

    while(1) {}
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

#ifdef TEST_FIFO_STREAM

FIFO io;

int main()
{
    uart_init(1, UART_BAUD_SELECT(9600, F_CPU));
    FILE *output = uart_open_stream(1);
    fifo_init(&io, 7);

    FILE *fifo_stream = fifo_open_stream(&io, &io);

    if (fgetc(fifo_stream) != EOF) {
        fputs("Expected EOF, FIFO is empty", output);
    }
    if (fgetc(fifo_stream) != EOF) {
        fputs("Expected EOF, FIFO is empty", output);
    }
    fputs("123456", fifo_stream);
    if (fputc('7', fifo_stream) != '7') {
        fputs("Expected char '7', FIFO is not full", output);
    }
    if (fputc('8', fifo_stream) != EOF) {
        fputs("Expected EOF because queue is full", output);
    }
    if (fgetc(fifo_stream) != '1') {
        fputs("Expected 1", output);
    }
    if (fgetc(fifo_stream) != '2') {
        fputs("Expected 2", output);
    }
    if (fgetc(fifo_stream) != '3') {
        fputs("Expected 3", output);
    }
    if (fgetc(fifo_stream) != '4') {
        fputs("Expected 4", output);
    }
    if (fgetc(fifo_stream) != '5') {
        fputs("Expected 5", output);
    }
    if (fgetc(fifo_stream) != '6') {
        fputs("Expected 6", output);
    }
    if (fgetc(fifo_stream) != '7') {
        fputs("Expected 7", output);
    }
    if (fgetc(fifo_stream) != EOF) {
        fputs("Expected EOF, FIFO is empty", output);
    }

    fputs("FIFO Test finished", output);
    while(1) {}
    return (1); // should never happen
}
#endif

#ifdef TEST_FBUS
FIFO io;
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
    uart_init(1, UART_BAUD_SELECT(115200, F_CPU));
    output = uart_open_stream(1);
    fputs("start tests\n\r", output);

    fifo_init(&io, 255);

    // test even data size
    // Frame: 0x1E, 0x00, 0x0C, 0x7F, 0x00, 0x02, 0xD2, 0x60, 0xC0, 0x1D

    uint8_t test_data[] = {0xD2, 0x01};
    fbus_init(fifo_open_stream(&io, &io));
    fbus_send_frame(0x7F, 0x0002, test_data);
    fbus_input_clear();
    assertEqualsUint8(FBUS_STATE_FRAME_ID_READ, fbus_read_frame(), "Expected FBUS_STATE_FRAME_ID_READ");
    assertEqualsUint8(FBUS_STATE_DEST_ADR_READ, fbus_read_frame(), "Expected FBUS_STATE_DEST_ADR_READ");
    assertEqualsUint8(FBUS_STATE_SRC_ADR_READ, fbus_read_frame(), "Expected FBUS_STATE_SRC_ADR_READ");
    assertEqualsUint8(FBUS_STATE_CMD_READ, fbus_read_frame(), "Expected FBUS_STATE_CMD_READ");
    assertEqualsUint8(0x7F, fbus_input_frame.command, "Expected 0x7F");
    assertEqualsUint8(FBUS_STATE_SIZE_MSB_READ, fbus_read_frame(), "Expected FBUS_STATE_SIZE_MSB_READ");
    assertEqualsUint16(0x0000, fbus_input_frame.data_size, "Expected 0x0000");
    assertEqualsUint8(FBUS_STATE_SIZE_LSB_READ, fbus_read_frame(), "Expected FBUS_STATE_SIZE_LSB_READ");
    assertEqualsUint16(0x0002, fbus_input_frame.data_size, "Expected 0x0002");
    assertEqualsUint8(FBUS_STATE_SIZE_LSB_READ, fbus_read_frame(), "Expected FBUS_STATE_SIZE_LSB_READ");
    assertEqualsUint8(0xD2, fbus_input_frame.data[0], "Expected 0xD2");
    assertEqualsUint8(FBUS_STATE_PADDING_BYTE_READ, fbus_read_frame(), "Expected FBUS_STATE_PADDING_BYTE_READ");
    assertEqualsUint8(0x60, fbus_input_frame.data[1], "Expected 0x60");
    assertEqualsUint8(FBUS_STATE_EVEN_CHK_READ, fbus_read_frame(), "Expected FBUS_STATE_EVEN_CHK_READ");
    assertEqualsUint8(FBUS_STATE_FRAME_READY, fbus_read_frame(), "Expected FBUS_STATE_FRAME_READY");
    assertEqualsUint8(0xC0, fbus_input_frame.even_checksum, "Expected 0xC0");
    assertEqualsUint8(0x1D, fbus_input_frame.odd_checksum, "Expected 0x1D");

    fputs("Finished test even data size\n\r", output);

    // test odd data size

    // Frame: 0x1E, 0x00, 0x0C, 0x7F, 0x00, 0x03, 0xD2, 0x01, 0x61, 0x00, 0xA1, 0x7D
    uint8_t test_data2[] = {0xD2, 0x01, 0x10};
    fbus_send_frame(0x7F, 0x0003, test_data2);
    fbus_input_clear();
    assertEqualsUint8(FBUS_STATE_FRAME_ID_READ, fbus_read_frame(), "Expected FBUS_STATE_FRAME_ID_READ");
    assertEqualsUint8(FBUS_STATE_DEST_ADR_READ, fbus_read_frame(), "Expected FBUS_STATE_DEST_ADR_READ");
    assertEqualsUint8(FBUS_STATE_SRC_ADR_READ, fbus_read_frame(), "Expected FBUS_STATE_SRC_ADR_READ");
    assertEqualsUint8(FBUS_STATE_CMD_READ, fbus_read_frame(), "Expected FBUS_STATE_CMD_READ");
    assertEqualsUint8(FBUS_STATE_SIZE_MSB_READ, fbus_read_frame(), "Expected FBUS_STATE_SIZE_MSB_READ");
    assertEqualsUint16(0x0000, fbus_input_frame.data_size, "Expected 0x0000");
    assertEqualsUint8(FBUS_STATE_SIZE_LSB_READ, fbus_read_frame(), "Expected FBUS_STATE_SIZE_LSB_READ");
    assertEqualsUint16(0x0003, fbus_input_frame.data_size, "Expected 0x0003");
    assertEqualsUint8(FBUS_STATE_SIZE_LSB_READ, fbus_read_frame(), "Expected FBUS_STATE_SIZE_LSB_READ");
    assertEqualsUint8(0xD2, fbus_input_frame.data[0], "Expected 0xD2");
    assertEqualsUint8(FBUS_STATE_SIZE_LSB_READ, fbus_read_frame(), "Expected FBUS_STATE_SIZE_LSB_READ");
    assertEqualsUint8(0x01, fbus_input_frame.data[1], "Expected 0x01");
    assertEqualsUint8(FBUS_STATE_DATA_READ, fbus_read_frame(), "Expected FBUS_STATE_DATA_READ");
    assertEqualsUint8(0x61, fbus_input_frame.data[2], "Expected 0x61");
    assertEqualsUint8(FBUS_STATE_PADDING_BYTE_READ, fbus_read_frame(), "Expected FBUS_STATE_PADDING_BYTE_READ");
    assertEqualsUint8(FBUS_STATE_EVEN_CHK_READ, fbus_read_frame(), "Expected FBUS_STATE_EVEN_CHK_READ");
    assertEqualsUint8(FBUS_STATE_FRAME_READY, fbus_read_frame(), "Expected FBUS_STATE_FRAME_READY");
    assertEqualsUint8(0xA1, fbus_input_frame.even_checksum, "Expected 0xA1");
    assertEqualsUint8(0x7D, fbus_input_frame.odd_checksum, "Expected 0x7D");

    fputs("Finished test odd data size\n\r", output);

    while(1) {

    }
    return (1); // should never happen
}

#endif

#ifdef TEST_LED
int main() {
    led_init();
    while(1) {
        led_on();
        _delay_ms(200);
        led_off();
        _delay_ms(800);
    }
}
#endif

#ifdef TEST_PHONE

uint8_t remote_number_octet[12] = {0x0c, 0x91, 0x94, 0x61, 0x23, 0x96, 0x34, 0x34, 0x00, 0x00, 0x00, 0x00};

int main() {
    debug_init();
    timer_init();
    mobile_init();
    sei();

    uint8_t mobile_state;

    mobile_state = mobile_on();

    if (mobile_state == MOBILE_READY) {
        mobile_state = mobile_send_sms(remote_number_octet, "Hello World!");
    }

    if (mobile_state == MOBILE_READY) {
        mdevice_sms.message[0] = 0x00;
        mobile_state = mobile_receive_sms();
        debug_printf("Received message: %s\n\r", (char *)mdevice_sms.message);
    }

    if (mobile_state == MOBILE_READY) {
        mobile_off();
    } else {
        debug_puts("Error!");
    }
    while(1) {}
}
#endif

#ifdef TEST_TIMER

void timer_test(void *data) {
    debug_puts(data);
}

int main() {
    led_init();
    debug_init();
    timer_init();
    sei();

    timer_start_timeout(0, &timer_test, "timer 1\n\r", 5000);
    timer_start_timeout(1, &timer_test, "timer 2\n\r", 100);
    timer_start_timeout(2, &timer_test, "timer 3\n\r", 50);
    timer_start_timeout(3, &timer_test, "timer 4\n\r", 3000);

    debug_puts("Expected order: 3, 2, 4\n\r");

    while(1) {}
}

#endif

#ifdef TEST_GSM
int main() {
    debug_init();
    sei();

    uint8_t message[23] = "This is a test message";
    uint8_t packed[22] = {};
    uint8_t packed_length = gsm_pack_7bit(packed, message, 22);
    debug_putc(packed_length);
    for (int i = 0; i < packed_length; ++i) {
        debug_putc(packed[i]);
    }
    while(1) {}
}
#endif
