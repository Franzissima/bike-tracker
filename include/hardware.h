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
#define DEBUG_UART                  1
#define DEBUG_BAUD                  UART_BAUD_SELECT(9600, F_CPU)
#define DEBUG_IN_BUF_SIZE           63
#define DEBUG_OUT_BUF_SIZE          63

/* timer */
#define TIMER_PRESCALE              (1<<CS01) | (1<<CS00) // prescaler = clk / 64
#define TIMER_COMPARE               58 - 1                // prescale and compare value result to 0.0010069444444444444 seconds
                                                          // between ticks at 3686400 Hz CPU Clock

#define TIMER_COUNT                 6

#define TIMER_UART_INDEX            0
#define TIMER_MDEVICE_INDEX         1
#define TIMER_MOBILE_INDEX          2
#define TIMER_BUZZER_INDEX          3
#define TIMER_MODE_SWITCH_INDEX     4
#define TIMER_MOT_DETECTION_INDEX   5

#define TRIGGER_COUNT               1

#define TRIGGER_BUZZER_INDEX        0

/* uart */
#define UART_TIMEOUT_MS             10000

/* buzzer */

#define BUZZER_PIN_1                (1<<PA0)
#define BUZZER_PIN_2                (1<<PA1)
#define BUZZER_DDR                  DDRA
#define BUZZER_PORT                 PORTA

/* led */

#define LED_PIN                     (1<<PB0)
#define LED_DDR                     DDRB
#define LED_PORT                    PORTB

/* mobile device */

#define NK6110
#define MDEVICE_UART                0
#define MDEVICE_BAUD                UART_BAUD_SELECT(115200, F_CPU)
#define MDEVICE_IN_BUF_SIZE         63
#define MDEVICE_OUT_BUF_SIZE        63
// minimum wait n milliseconds between receiving power on frames from phone
// and starting initialization sequence for first frame send
#define MDEVICE_POWER_ON_DELAY_MS   500
#define MDEVICE_TIMEOUT_MS          8000

/* mobile */
#define MOBILE_POWER_ON_PIN         (1<<PC0)
#define MOBILE_POWER_ON_DDR         DDRC
#define MOBILE_POWER_ON_PORT        PORTC
#define MOBILE_POWER_SWITCH_MS      5000
#define MOBILE_POWER_SWITCH_WAIT_TRIGGER_MS 500 // wait between on/off trigger of power switch

#define MOBILE_ERROR_RETRY_COUNT    5
#define MOBILE_SMS_SEND_RETRY_COUNT 1
#define MOBILE_WAIT_SIM_READY_MS    500
#define MOBILE_WAIT_SIM_READY_COUNT 20

#define MOBILE_PIN {0x31, 0x32, 0x33, 0x34}

/* fbus */
#define FBUS_MAX_DATA_LENGTH        256

/* mode switch */
//#define MODE_SWITCH_ENABLE_PULL_UP //enable this if you have an opener reed contact
#define MODE_SWITCH_MAX_MODE        2
#define MODE_SWITCH_PIN             PCINT23_PIN
#define MODE_SWITCH_DDR             PCINT23_DDR
#define MODE_SWITCH_PORT            PCINT23_PORT
#define MODE_SWITCH_BIT             (1<<PCINT23_BIT)
#define MODE_SWITCH_PCIE            (1<<PCIE2)
#define MODE_SWITCH_PCIF            (1<<PCIF2)
#define MODE_SWITCH_PCMSK           PCMSK2
#define MODE_SWITCH_VECT            PCINT2_vect
#define MODE_SWITCH_DEBOUNCE_MS     100
#define MODE_SWITCH_TIMEOUT_MS      2000
#define MODE_SWITCH_LONG_BEEP_MS    1500
#define MODE_SWITCH_SHORT_BEEP_MS   500
#define MODE_SWITCH_BEEP_PAUSE_MS   300

/* motion detection */
#define MOTION_DETECTION_PIN        PCINT31_PIN
#define MOTION_DETECTION_DDR        PCINT31_DDR
#define MOTION_DETECTION_PORT       PCINT31_PORT
#define MOTION_DETECTION_BIT        (1<<PCINT31_BIT)
#define MOTION_DETECTION_PCIE       (1<<PCIE3)
#define MOTION_DETECTION_PCIF       (1<<PCIF3)
#define MOTION_DETECTION_PCMSK      PCMSK3
#define MOTION_DETECTION_VECT       PCINT3_vect

#endif /* HARDWARE_H_ */
