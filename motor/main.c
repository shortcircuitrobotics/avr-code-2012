/*
 * main.c
 *
 *  Created on: May 18, 2012
 *      Author: kevin
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "libs/uart_async.h"
#include "libs/sys_timer.h"
#include "peripherals/compass.h"
#include "peripherals/motors.h"


int main(void){
	uart_async_init(BAUDRATE);
	uart_endl();
	uart_writeline("--------------- Motor Test ---------------");
	uart_writeline("- Version 0.2 by Kevin Laeufer           -");
	uart_writeline("- Last Modified July 01 2012             -");
	uart_writeline("------------------------------------------");
	uart_async_send_all();

	DDRB |= (1<<PB7);
	PORTB |= (1<<PB7);
	// Initialize Hardware
	motors_init();		// Motors
						// SPI
	_delay_ms(1);		// wait for compass to be ready
	compass_init();		// Compass
	sys_timer_init();	// System Timer

	sys_timer_set_compass_timeout(200);	// read compass every 500 ms
	sei();				// enable global interrupts

	motor.turn_to = 1;
	motor.angle = 90;
	motor.speed = 0;

	/*motor_A_set_clockwise();
	motor_A_set_speed(20);

	motor_C_set_clockwise();
	motor_C_set_speed(20);*/


	while(1){
		// Run FSM
		compass_run();
		uart_async_run();
		// Check if there is new compass data
		if(compass_new_heading()){
			uart_send_word(compass_read_heading()/10, "New Heading:");
			uart_endl();
		}
	}

	return 0;
}



