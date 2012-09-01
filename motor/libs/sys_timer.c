/*
 * sys_timer.c
 *
 *  Created on: Jul 1, 2012
 *      Author: kevin
 */

#include "sys_timer.h"
#include "../peripherals/compass.h"
#include "../peripherals/motors.h"
#include <avr/interrupt.h>

uint32_t system_time = 0;
uint16_t compass_time_out = 200;
uint16_t compass_counter = 0;

uint16_t motor_time_out = 10;
uint16_t motor_counter = 0;

/*
 * Initialize Timer2 to function as system timer
 */
void sys_timer_init(){
	// Start Timer 1
	TCCR1B = (1<<WGM12) | (1<<CS12);	// clk/256 => 62.5kHZ
	OCR1A = 63; 						// => ~p=1ms
	TIMSK1 = (1<<OCIE1A);	// enable compare match interrupt
	system_time = 0;		// reset system time
	compass_counter = 0;
}

void sys_timer_init_motor_timer(uint16_t motor_ms){
	motor_time_out = motor_ms;
	motor_counter = 0;
}

void sys_timer_set_compass_timeout(uint16_t ms){
	compass_time_out = ms;
	compass_counter = 0;
}

ISR(TIMER1_COMPA_vect ){  // is called on compare match
	system_time++;
	compass_counter++;
	motor_counter++;

	if(motor_counter == motor_time_out){
		//PORTB ^= (1<<PB7);
		motors_update();
		motor_counter = 0;
	}
	if(compass_counter == compass_time_out){

		compass_start();	// start compass fsm
		compass_counter = 0;
	}
}
