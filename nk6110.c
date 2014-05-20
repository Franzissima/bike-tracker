/**
 * Implementation of mobile device API for nokia 6110 based handies.
 * See gnokii project for details: www.gnokii.org, file nk6110.txt
 */
#include <avr/interrupt.h>
#include <util/delay.h>
#include "include/mdevice.h"
#include "include/uart.h"
#include "include/fbus.h"

#ifdef NK6110

#define COMMAND_STATUS                        0x04
#define COMMAND_CODE                          0x08
#define COMMAND_NETWORK_STATUS                0x0a
#define COMMAND_TX_GET_HARDWARE_VERSION       0xd1
#define COMMAND_RC_HARDWARE_VERSION           0xd2

uint8_t mdevice_state = MDEVICE_STATE_OFF;

uint8_t mdevice_tx_command;
uint8_t mdevice_rc_expected_command;

void mdevice_init() {
    // initialize UART
    uart_async_init(MDEVICE_UART, MDEVICE_BAUD, MDEVICE_IN_BUF_SIZE, MDEVICE_OUT_BUF_SIZE);
    fbus_init(uart_async_open_stream(MDEVICE_UART, 0));
	fbus_input_clear();
}

void _mdevice_send_acknowledge(uint8_t rc_command) {
    // send acknowledge
    uint8_t received_sequence = fbus_input_frame.data[fbus_input_frame.data_size - 1] & 0x0F;
    uint8_t cmd_data[2];
    cmd_data[0] = rc_command;
    cmd_data[1] = received_sequence;
    fbus_send_frame(FBUS_COMMAND_ACKNOWLEDGE, 2, cmd_data);
}

uint8_t _mdevice_process_state(FILE *debug) {
    uint8_t command = fbus_input_frame.command;
    switch (mdevice_state) {
    case MDEVICE_STATE_OFF:
         //Nokia 3310 sends power up commands:
         //1e ff 00 d0 00 03 01 01 e0 00 ff 2d  First Command
         //1e 14 00 f4 00 01 03 00 1d e1        Second Command
        fbus_input_clear();
        if (command == 0xf4) { // second power on frame received
            // now wait for power up before initialization
            _delay_ms(MDEVICE_POWER_ON_DELAY_MS);
            // Receiving of these two frames is no indicator for end of power-on-pulse!
            mdevice_state = MDEVICE_STATE_READY;
        }
        break;
    case MDEVICE_STATE_WAIT_FOR_ACK:
        if (command == FBUS_COMMAND_ACKNOWLEDGE) {
            //Example acknowledge command send by phone:
            //1e 0c 00 7f 00 02 d1 00 cf 71
            if (fbus_input_frame.data[0] != mdevice_tx_command) {
                fputs("Error: Received acknowledge for unexpected command\n\r", debug);
                mdevice_state = MDEVICE_STATE_ERROR;
            } else {
                fputs("Received acknowledge\n\r", debug);
                fbus_input_clear();
                mdevice_state = MDEVICE_STATE_WAIT_FOR_RESPONSE;
            }
        } else {
            // unexpected phone response
            fprintf(debug, "Warning: Expected acknowledge but got %#.2x\n\r", command);
            fbus_dump_frame(debug);
            // this might be some status frame, send acknowledge to keep in sync with phone
            _mdevice_send_acknowledge(command);
            fbus_input_clear();
        }
        break;
    case MDEVICE_STATE_WAIT_FOR_RESPONSE:
        if (command == mdevice_rc_expected_command) {
            fputs("Received response, send acknowledge\n\r", debug);
            // send acknowledge
            _mdevice_send_acknowledge(command);
            mdevice_state = MDEVICE_STATE_RESPONSE_READY;
        } else {
            // unexpected phone response
            fprintf(debug, "Error: Phone sends unexpected response: %#.2x\n\r", command);
            mdevice_state = MDEVICE_STATE_ERROR;
        }
        break;
    case MDEVICE_STATE_RESPONSE_READY:
        fprintf(debug, "Received message from phone: %#.2x\n\r", command);
        break;
    default:
        break;
    }
    return mdevice_state;
}

uint8_t mdevice_process(FILE *debug) {
    uint8_t fbus_state = fbus_read_frame();
    if (IS_FBUS_ERROR()) {
        fputs("fbus error", debug);
        fbus_input_clear();
        mdevice_state = MDEVICE_STATE_ERROR;
    } else if (IS_FBUS_READY()) {
        _mdevice_process_state(debug);
    } else if (fbus_state != FBUS_STATE_INPUT_QUEUE_EMPTY) {
        fprintf(debug, "fbus state: %#.2x\n\r", fbus_state);
    }
    return mdevice_state;
}

void mdevice_tx_get_status() {
    fbus_input_clear();
    uint8_t req[] = {FBUS_FRAME_HEADER, 0x01};
    fbus_send_frame(COMMAND_STATUS, 4, req);
    mdevice_tx_command = COMMAND_STATUS;
    mdevice_rc_expected_command = COMMAND_STATUS;
    mdevice_state = MDEVICE_STATE_WAIT_FOR_ACK;
}

uint8_t mdevice_get_status() {
    return fbus_input_frame.data[2];
}

void mdevice_tx_get_hdw_version() {
    fbus_input_clear();
    uint8_t req[] = {FBUS_FRAME_HEADER, 0x03, 0x00, 0x01, 0x00};
    fbus_send_frame(COMMAND_TX_GET_HARDWARE_VERSION, 7, req);
    mdevice_tx_command = COMMAND_TX_GET_HARDWARE_VERSION;
    mdevice_rc_expected_command = COMMAND_RC_HARDWARE_VERSION;
    mdevice_state = MDEVICE_STATE_WAIT_FOR_ACK;
}

uint8_t *mdevice_get_hdw_version() {
    return fbus_input_frame.data + 4;
}

void mdevice_rc_wait_for_network_status() {
    fbus_input_clear();
    mdevice_rc_expected_command = COMMAND_NETWORK_STATUS;
    mdevice_state = MDEVICE_STATE_WAIT_FOR_RESPONSE;
}

void mdevice_tx_get_pin_status() {
    fbus_input_clear();
    uint8_t req[] = {FBUS_FRAME_HEADER, 0x07, 0x01, 0x01, 0x00};
    fbus_send_frame(COMMAND_CODE, 7, req);
    mdevice_tx_command = COMMAND_CODE;
    mdevice_rc_expected_command = COMMAND_CODE;
    mdevice_state = MDEVICE_STATE_WAIT_FOR_ACK;
}

void mdevice_tx_enter_pin(uint8_t pin[4]) {
    //1e 00 0c 08 00 0d    00 01 00 0a 02 31 32 33 34 00 00 - 01 - 46 - 00 - 50 0d
    fbus_input_clear();
    uint8_t req[] = {FBUS_FRAME_HEADER, 0x0a, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00};
    req[5] = pin[0];
    req[6] = pin[1];
    req[7] = pin[2];
    req[8] = pin[3];
    fbus_send_frame(COMMAND_CODE, 13, req);
    mdevice_tx_command = COMMAND_CODE;
    mdevice_rc_expected_command = COMMAND_CODE;
    mdevice_state = MDEVICE_STATE_WAIT_FOR_ACK;
}

uint8_t mdevice_get_pin_status() {
    switch (fbus_input_frame.data[3]) {
    case 0x05:
        return MDEVICE_PIN_CHANGE_OK;
    case 0x06:
    case 0x09:
    case 0x0c:
        switch (fbus_input_frame.data[4]) {
        case 0x6f:
        case 0x79:
            return MDEVICE_PIN_SIM_CARD_NOT_READY;
        case 0x88: // or: code not needed?
        case 0x8d:
            return MDEVICE_PIN_WRONG_PIN_CODE;
        }
        break;
    case 0x08:
        switch (fbus_input_frame.data[4]) {
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
            return MDEVICE_PIN_WAIT_FOR;
        }
        break;
    case 0x0b:
        return MDEVICE_PIN_ACCEPTED;
    }
    return MDEVICE_PIN_UNKNOWN;
}
#endif
