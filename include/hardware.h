/*
 * hardware.h
 *
 *  Created on: 27.04.2014
 *      Author: andreasbehnke
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

/* debug */
#define DEBUG /* turn debugging on/of */
#define DEBUG_UART                1
#define DEBUG_BAUD                UART_BAUD_SELECT(9600, F_CPU)
#define DEBUG_IN_BUF_SIZE         63
#define DEBUG_OUT_BUF_SIZE        63

/* timer */
#define TIMER_PRESCALE            (1<<CS01) | (1<<CS00) // prescaler = clk / 64
#define TIMER_COMPARE             58 - 1                // prescale and compare value result to 0.0010069444444444444 seconds
                                                        // between ticks at 3686400 Hz CPU Clock

#define TIMER_COUNT                     3

#define TIMER_UART_INDEX                0
#define TIMER_MDEVICE_INDEX             1
#define TIMER_MOBILE_INDEX              2

/* uart */
#define UART_TIMEOUT_MS           10000

/* buzzer */

#define BUZZER_PIN_1              (1<<PA0)
#define BUZZER_PIN_2              (1<<PA1)
#define BUZZER_DDR                DDRA
#define BUZZER_PORT               PORTA

/* led */

#define LED_PIN                   PB0
#define LED_DDR                   DDRB
#define LED_PORT                  PORTB

/* mobile device */

#define NK6110
#define MDEVICE_UART              0
#define MDEVICE_BAUD              UART_BAUD_SELECT(115200, F_CPU)
#define MDEVICE_IN_BUF_SIZE       31
#define MDEVICE_OUT_BUF_SIZE      31
// minimum wait n milliseconds between receiving power on frames from phone
// and starting initialization sequence for first frame send
#define MDEVICE_POWER_ON_DELAY_MS 500
#define MDEVICE_TIMEOUT_MS        8000

/* mobile */

#define MOBILE_POWER_ON_PIN         (1<<PC0)
#define MOBILE_POWER_ON_DDR         DDRC
#define MOBILE_POWER_ON_PORT        PORTC
#define MOBILE_POWER_SWITCH_MS      5000
#define MOBILE_POWER_SWITCH_WAIT_TRIGGER_MS 500 // wait between on/off trigger of power switch

#define MOBILE_ERROR_RETRY_COUNT    5
#define MOBILE_WAIT_SIM_READY_MS    500
#define MOBILE_WAIT_SIM_READY_COUNT 20

#define MOBILE_PIN {0x31, 0x32, 0x33, 0x34}

/* fbus */

#define FBUS_MAX_DATA_LENGTH 256

#endif /* HARDWARE_H_ */
