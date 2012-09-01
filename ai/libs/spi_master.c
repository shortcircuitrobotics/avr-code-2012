/*
 * spi_master.c
 *
 *  Created on: 07.08.2012
 *      Author: christoph
 */


#include <avr/io.h>
#include "spi_master.h"

struct{
	enum{
		IDLE,
		SENDING,
		RECEIVING
	}send_receive;
}state;

static inline void ss_high(uint8_t chip){
	if(chip==SENSOR_SS){
		PORTA |= (1<<PA7);
	}
	if(chip==MOTOR_SS){
		PORTB |= (1<<PB0);
	}
	if(chip==MOUSESENSOR_SS){
		PORTA |= (1<<PA4);
	}
	if(chip==LCD_SS){
		PORTB |= (1<<PB2);
	}
	if(chip==CPLD_SS){
		PORTB |= (1<<PB1);
	}
}

static inline void ss_low(uint8_t chip){
	if(chip==SENSOR_SS){
		PORTA &= ~(1<<PA7);
	}
	if(chip==MOTOR_SS){
		PORTB &= ~(1<<PB0);
	}
	if(chip==MOUSESENSOR_SS){
		PORTA &= ~(1<<PA4);
	}
	if(chip==LCD_SS){
		PORTB &= ~(1<<PB2);
	}
	if(chip==CPLD_SS){
		PORTB &= ~(1<<PB1);
	}
}

void spi_master_init(){
	// initialize SS ports (PB0, PB1, PB2, PA4, PA7)
	DDRB |= (1<<PB0) | (1<<PB1) | (1<<PB2);
	DDRA |= (1<<PA4) | (1<<PA7);
	DDRB |= (1<<PB5);
	DDRB |= (1<<PB4); //set PB4 output temporary
	SPCR = (1<<SPE | 1<<MSTR | 1<<SPR1 | 1<<SPR0);
	DDRB &= ~(1<<PB4); //set PB4 input for button again
}

uint8_t spi_send_receive(uint8_t send_data, uint8_t* received_data, uint8_t chipselect){
	switch(state.send_receive){
	case IDLE:
		ss_high(chipselect);
		ss_low(chipselect);
		SPDR = send_data;
		state.send_receive = SENDING;
		break;
	case SENDING:
		if(!(SPSR & (1<<SPIF))){
			state.send_receive = RECEIVING;
		}
		break;
	case RECEIVING:
		*received_data = SPDR;
		ss_high(chipselect);
		state.send_receive = IDLE;
		break;
	}
	return state.send_receive;
}

