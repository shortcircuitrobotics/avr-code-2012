/*
 * motors.c
 *
 *  Created on: Jul 1, 2012
 *      Author: kevin
 */

#include <avr/interrupt.h>
#include "motors.h"
#include "../libs/sys_timer.h"
#include "../libs/trig_int.h"
#include "compass.h"

#define KP 1/10
#define KD 1

#define TURN_SPEED_MAX 30
#define PLUS_MINUS_ERROR 2

#define set_max(value, max) { if(value > max) value = max; else if(value < -max) value = -max; }


motor_data motor;

/*
 * Initializes the hardware used to interface to the motor controllers
 */

int16_t static inline error_(uint16_t a1, uint16_t a2){
	if(abs_(a2-a1) <= 180){
		return a2 - a1;
	}
	else {
		if(a1 > a2)
			return 360 - (a1 - a2);
		else
			return (a2 - a1) - 360;
	}
}

void motors_init(){
	// Motor Controller A
	// CS (PC0/ADC0), EN (PD6/OC0A), IN_A (PB0), INB (PB1),
	TCCR0A |= (1<<COM0A1) | (1<<WGM01) | (1<<WGM00); 	// fast pwm on OC0A
	TCCR0B |= (1<<CS01);// | (1<<CS00);	// start timer; clk/8
	OCR0A = 0x00;	// duty cycle
	DDRB |= (1<<PB1) | (1<<PB0);	// set A and B output
	DDRD |= (1<<PD6);				// PWM out needs to be output too

	// Motor Controller B
	// CS (PC1/ADC1), EN (PD5/OC0B), IN_A (PD2), INB (PC3),
	TCCR0A |= (1<<COM0B1); 	// fast pwm on OC0B
	OCR0B = 0x00;	// duty cycle
	DDRC |= (1<<PC3);				// set  B output
	DDRD |= (1<<PD5) | (1<<PD2);	// PWM and A as output
	PORTD |= (1<<PD2);

	// Motor Controller C
	// CS (PC2/ADC2), EN (PD3/OC2B), IN_A (PD4), INB (PD7),
	TCCR2A |= (1<<COM2B1) | (1<<WGM21) | (1<<WGM20); 	// fast pwm on OC2B
	TCCR2B |= (1<<CS21);								// start timer; clk/8
	OCR2B = 0x00;	// duty cycle
	DDRD |= (1<<PD7) | (1<<PD4) | (1<<PD3);	// PWM, A and B as output
	PORTD |= (1<<PD4);

	// Setup motor timer (using system timer)
	sys_timer_init_motor_timer(10); // call motors_update every 10ms => 100Hz
}


void motors_update(){

	if(motor.drive && !motor.turn_to){

		int16_t speed_a, speed_b, speed_c;

		//normal calculations
	    speed_a = (int16_t)(-((int32_t)motor.speed * (int32_t)cos_(abs_(150 - motor.angle))) / TRIG_INT_SIN_90);
	    speed_b = (int16_t)(-((int32_t)motor.speed * (int32_t)cos_(abs_(270 - motor.angle))) / TRIG_INT_SIN_90);
	    speed_c = (int16_t)(-((int32_t)motor.speed * (int32_t)cos_(abs_(30 - motor.angle))) / TRIG_INT_SIN_90);

		//Motor A
		if(speed_a<0 && speed_a>-256){
			motor_A_set_counter_clockwise();
			motor_A_set_speed(-speed_a);
		}
		else if(speed_a>0 && speed_a<256){
			motor_A_set_clockwise();
			motor_A_set_speed(speed_a);
		}
		else{
			motor_A_break();
			motor_A_set_speed(0);
		}

		//Motor B
		if(speed_b<0 && speed_b>-256){
			motor_B_set_counter_clockwise();
			motor_B_set_speed(-speed_b);
		}
		else if(speed_b>0 && speed_b<256){
			motor_B_set_clockwise();
			motor_B_set_speed(speed_b);
		}
		else{
			motor_B_break();
			motor_B_set_speed(0);
		}

		// Motor C
		if(speed_c<0 && speed_c>-256){
			motor_C_set_counter_clockwise();
			motor_C_set_speed(-speed_c);
		}
		else if(speed_c>0 && speed_c<256){
			motor_C_set_clockwise();
			motor_C_set_speed(speed_c);
		}
		else{
			motor_C_break();
			motor_C_set_speed(0);
		}
	}

	else if(motor.turn_to && !motor.drive && !comp_invalid){
		int16_t error, delta_error, p_out, d_out, speed;

		error = error_(compass_read_heading()/10, motor.angle);
		delta_error = error - pid.prev_err;

		if((abs_(error) <= PLUS_MINUS_ERROR) && abs_(delta_error) < 2){
			motor.turn_to=0;
		}

		p_out = error * KP;
		d_out = delta_error * KD;

		speed = motor.speed + p_out + d_out;
		set_max(speed, TURN_SPEED_MAX);

		if(speed > 0){
			motor_all_set_counter_clockwise();
			motor_all_set_speed(speed);
		}
		else if(speed < 0){
			motor_all_set_clockwise();
			motor_all_set_speed(-speed);
		}
		else{
			motor_all_break();
		}

	}
	else{
		motor_all_break();
	}

}

