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
#include "peripherals/adc.h"
#include "peripherals/tsop.h"


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

	DDRD |= (1<<PD3);



	while(1){
		// Run FSM
		uart_async_run();
		adc_start_read(tsop_data);
		while(tsops_run());


	}

	return 0;
}


