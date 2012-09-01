/*
 * dogm.h
 *
 *  Created on: Jun 25, 2012
 *      Author: kevin
 */

#ifndef DOGM_H_
#define DOGM_H_

#include <avr/io.h>

#define SS_LCD_high() PORTB |=  (1<<PB2)
#define SS_LCD_low()  PORTB &= ~(1<<PB2)
#define A0_high() PORTD |=  (1<<PD5)
#define A0_low()  PORTD &= ~(1<<PD5)
#define RST_LCD_high() PORTB |=  (1<<PB4)
#define RST_LCD_low()  PORTB &= ~(1<<PB4)


void dogm_init();


#endif /* DOGM_H_ */
