/*
 * main.c
 *
 *  Created on: May 18, 2012
 *      Author: kevin
 */

#include <avr/io.h>
#include <util/delay.h>

#include "libs/dogm.h"
#include "libs/uart.h"

//#include "libs/spi_master.h"
#include "peripherals/dribbler.h"

#define SS_SENSOR_high() PORTA |=  (1<<PA7)
#define SS_SENSOR_low()  PORTA &= ~(1<<PA7)
#define SS_MOTOR_high() PORTB |=  (1<<PB0)
#define SS_MOTOR_low()  PORTB &= ~(1<<PB0)

int main(void){

	//spi_master_init();// enable spi; set outputs
	dogm_init();

	// initialize uart
	uart_init(BAUDRATE);


	// Eanble Light Barrier PWM on PB3 (OC0A)
	DDRB |= (1<<PB3);
	TCCR0A = (1<<COM0A0) | (1<<WGM01) | (1<<WGM00);	// Fast PWM with OCR0A
	OCR0A = 42;	// => ~20kHz (@ 16MHz)
	TCCR0B = (1<<WGM02) | (1<<CS01);	// start timer

	dribbler_init();

	// Kicker: PA6
	DDRA |= (1<<PA6);
	//_delay_ms(1000);
	// Trigger Kicker once
	/*PORTA |= (1<<PA6);
	_delay_ms(50);
	PORTA &= ~(1<<PA6);*/
	//uart_puts("TEST");

	PORTA |= (1<<PA0);		//fwd
	PORTA |= (1<<PA1);		//break off
	OCR2A = 0xF0;			//speed F0

	while(1){
		// try communicating with motor controller
		/*SS_MOTOR_low();
		SPDR = 0x33;
		while(!(SPSR & (1<<SPIF)));
		SS_MOTOR_high();
		for(uint8_t i = 0; i < 8; i++){
			asm("nop");
		}
		SS_MOTOR_low();
		SPDR = 0x55;
		while(!(SPSR & (1<<SPIF))); = IDLE;
		}
		SS_MOTOR_high();
		uart_send(SPDR);
		_delay_ms(200);
		while(spi_send_receive(0xB2, nix, SENSOR_SS));
		while(spi_send_receive(0x00, right, SENSOR_SS));
		uart_send(*right);*/



	}
	return 0;
}
