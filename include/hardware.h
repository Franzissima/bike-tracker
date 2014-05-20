/*
 * hardware.h
 *
 *  Created on: 27.04.2014
 *      Author: andreasbehnke
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

/* turn debugging on/of */
#define DEBUG
#define DEBUG_UART                1
#define DEBUG_BAUD                UART_BAUD_SELECT(115200, F_CPU)
#define DEBUG_IN_BUF_SIZE         63
#define DEBUG_OUT_BUF_SIZE        63

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
#define MDEVICE_IN_BUF_SIZE       0xFF
#define MDEVICE_OUT_BUF_SIZE      0xFF
// minimum wait n milliseconds between receiving power on frames from phone
// and starting initialization sequence for first frame send
#define MDEVICE_POWER_ON_DELAY_MS 500

/* mobile */

#define MOBILE_POWER_ON_PIN       (1<<PC0)
#define MOBILE_POWER_ON_DDR       DDRC
#define MOBILE_POWER_ON_PORT      PORTC

/* fbus */

#define FBUS_MAX_DATA_LENGTH 256

#endif /* HARDWARE_H_ */
