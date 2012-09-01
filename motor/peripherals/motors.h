/*
 * motors.h
 *
 *  Created on: Jul 1, 2012
 *      Author: kevin
 *
 *
 *      This file (together with motors.c) contains the code necessary to control
 *      the three driving motors of this robot.
 *
 *      Hardware Resources used:
 *      -
 */

#ifndef MOTORS_H_
#define MOTORS_H_

#include <avr/io.h>

typedef struct{
	int16_t angle;
	int16_t speed;
	struct{
		unsigned drive :1;
		unsigned turn_to :1;
		unsigned :6;
	};
} motor_data;

struct{
	int16_t prev_err;
}pid;

extern motor_data motor;


#define motor_A_set_clockwise() 			{ PORTB |= (1<<PB0); PORTB &= ~(1<<PB1); }
#define motor_A_set_counter_clockwise() 	{ PORTB &= ~(1<<PB0); PORTB |= (1<<PB1); }
#define motor_A_break() 					{ PORTB &= ~(1<<PB0); PORTB &= ~(1<<PB1); }
#define motor_A_set_speed(x)				OCR0A = x

#define motor_B_set_clockwise() 			{ PORTD |= (1<<PD2); PORTC &= ~(1<<PC3); }
#define motor_B_set_counter_clockwise() 	{ PORTD &= ~(1<<PD2); PORTC |= (1<<PC3); }
#define motor_B_break() 					{ PORTD &= ~(1<<PD2); PORTC &= ~(1<<PC3); }
#define motor_B_set_speed(x)				OCR0B = x

#define motor_C_set_clockwise() 			{ PORTD |= (1<<PD4); PORTD &= ~(1<<PD7); }
#define motor_C_set_counter_clockwise() 	{ PORTD &= ~(1<<PD4); PORTD |= (1<<PD7); }
#define motor_C_break() 					{ PORTD &= ~(1<<PD4); PORTD &= ~(1<<PD7); }
#define motor_C_set_speed(x)				OCR2B = x

#define motor_all_set_clockwise()	{ motor_A_set_clockwise(); motor_B_set_clockwise(); motor_C_set_clockwise(); }
#define motor_all_set_counter_clockwise() { motor_A_set_counter_clockwise(); motor_B_set_counter_clockwise(); motor_C_set_counter_clockwise(); }
#define motor_all_break() { motor_A_break(); motor_B_break(); motor_C_break(); motor_all_set_speed(0); }
#define motor_all_set_speed(x)				{ motor_A_set_speed(x); motor_B_set_speed(x); motor_C_set_speed(x); }

void motors_init();	/* Initializes the hardware used to interface to the motor controllers*/
void motors_update();

#endif /* MOTORS_H_ */
