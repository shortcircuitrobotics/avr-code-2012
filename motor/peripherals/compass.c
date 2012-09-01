/*
 * compass.c
 *
 *  Created on: Jul 1, 2012
 *      Author: kevin
 */

#include "../libs/i2c.h"
#include "../libs/uart_async.h"
#include "compass.h"


struct{
	uint16_t heading;
	struct{
		unsigned connected :1;
		unsigned read_heading :1;
		unsigned new_heading :1;
	};
	enum{
		ST_IDLE,
		ST_SENDING_COMMAND,
		ST_READING_HEADING
	} state;
} comp;

// prototype
uint8_t static compass_read_eeprom(uint8_t address);

void compass_init(){
	// Initialize I2C Library
	i2c_async_init(SCL_CLOCK);
	// Initialize Global Variables
	comp.new_heading = 0;
	comp.read_heading = 0;
	comp.state = ST_IDLE;
	// Read address to check if compass is connected
	uint8_t address = compass_read_eeprom(0x00);
	if(address == HMC6352_ADDRESS){
		comp.connected = 1;
	}
	else{
		comp.connected = 0;
		uart_writeline("Error: HMC6352 not connected!");
		uart_async_send_all();
	}
}

void compass_start(){
	if(comp.connected){
		comp.read_heading = 1;	// set flag
	}
}

void compass_run(){
	uint16_t temp;
	switch(comp.state){
	case ST_IDLE:
		if(comp.read_heading){	// if start flag is set
			comp.read_heading = 0;
			i2c_async_start(HMC6352_ADDRESS | I2C_WRITE);	// setup i2c FSM
			i2c_async_write(HMC6352_READ_HEADING);
			comp.state = ST_SENDING_COMMAND;
		}
		break;
	case ST_SENDING_COMMAND:
		if(!i2c_async_run()){	// if i2c is done
			i2c_async_start(HMC6352_ADDRESS | I2C_READ);	// read heading
			i2c_async_read_word();
			i2c_async_stop();
			comp.state = ST_READING_HEADING;	// go to next state
		}
		break;
	case ST_READING_HEADING:
		if(!i2c_async_run()){	// if i2c is done
			temp = i2c_async_get_word();
			if(temp != comp.heading){
				comp.heading = temp;
				comp.new_heading = 1;
			}
			comp.state = ST_IDLE;	// return to IDLE
		}
		break;
	default:
		comp.state = ST_IDLE;
		break;
	}
}

bool compass_new_heading(){
	return comp.new_heading;
}

uint16_t compass_read_heading(){
	comp.new_heading = 0;
	return comp.heading;
}

uint8_t static compass_read_eeprom(uint8_t address){
	i2c_async_start(HMC6352_ADDRESS | I2C_WRITE);
	i2c_async_write_2(HMC6352_READ_EEPROM, address);
	while(i2c_async_run());

	i2c_async_start(HMC6352_ADDRESS | I2C_READ);
	i2c_async_read_nak();
	i2c_async_stop();
	while(i2c_async_run());

	return i2c_async_get_byte();
}
