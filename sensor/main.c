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
#include "libs/spi_slave.h"
#include "libs/communication.h"


int main(void){
	uart_async_init(BAUDRATE);
	uart_endl();
	uart_writeline("--------------- Sensor Test ---------------");
	uart_writeline("- Version 0.2 by Kevin Laeufer           -");
	uart_writeline("- Last Modified July 01 2012             -");
	uart_writeline("------------------------------------------");
	uart_async_send_all();

	// Initialize Hardware
	spi_slave_init(&send_data, &spi_end);	// SPI
	_delay_ms(1);		// wait for compass to be ready
	//sys_timer_init();	// System Timer
	//sys_timer_set_compass_timeout(200);	// read compass every 500 ms
	//sei();				// enable global interrupts

	DDRD |= (1<<PD3);

	while(1){
		// Run FSM
		uart_async_run();

		PORTD |= (1<<PD3);
		_delay_us(15);
		PORTD &= ~(1<<PD3);
		_delay_ms(200);

		// Check if there is new compass data
		//if(compass_new_heading()){
		//	uart_send_word(10, "Distance (cm):");
		//	uart_endl();
		//}
	}


	while(1){}

	return 0;
}



