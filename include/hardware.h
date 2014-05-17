/*
 * hardware.h
 *
 *  Created on: 27.04.2014
 *      Author: andreasbehnke
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

/* buzzer */

#define BUZZER_PIN_1       (1<<PA0)
#define BUZZER_PIN_2       (1<<PA1)
#define BUZZER_DDR         DDRA
#define BUZZER_PORT        PORTA

/* led */
#define LED_PIN            PB0
#define LED_DDR            DDRB
#define LED_PORT           PORTB

/* phone */
#define PHONE_UART              0
#define PHONE_BAUD              UART_BAUD_SELECT(115200, F_CPU)
#define PHONE_IN_BUF_SIZE       0xFF
#define PHONE_OUT_BUF_SIZE      0xFF
// minimum wait n milliseconds between receiving power on frames from phone
// and starting initialization sequence for first frame send
#define PHONE_POWER_ON_DELAY_MS 500
// minimum wait for SIM card ready
#define PHONE_SIM_CARD_READY_DELAY_MS 5000
#endif /* HARDWARE_H_ */
