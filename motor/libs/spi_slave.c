/*
 * spi.c
 *
 *  Created on: Jun 26, 2012
 *      Author: kevin
 */

#include <avr/interrupt.h>
#include "spi_slave.h"

// These defines need to be changed in order to make this
// library work with other AVR microcontrollers
#define spi_enable_mosi() DDRB |= (1<<PB4)
#define spi_enable() SPCR = (1<<SPIE) | (1<<SPE)
#define spi_send(x) SPDR = x
#define spi_read() SPDR

struct{
	spi_slave_start_ptr start;
	spi_slave_end_ptr end;
	uint8_t buffer[SPI_SLAVE_BUFFER_LENGTH];
	uint8_t buffer_index;
	uint8_t buffer_length;
	uint8_t header;
	enum{
		ST_IDLE,
		ST_TRANSMITTING
	} state;
} spi;

// Called at the end of a transmission
ISR(SPI_STC_vect){
	switch(spi.state){
	case ST_IDLE:
		spi.header = spi_read();
		spi.buffer_length = spi.start(spi.header, spi.buffer);
		spi.buffer_index = 0;
		spi_send(spi.buffer[spi.buffer_index]);
		spi.state = ST_TRANSMITTING;
		break;
	case ST_TRANSMITTING:
		spi.buffer[spi.buffer_index] = spi_read();
		spi.buffer_index++;
		if(spi.buffer_index <  spi.buffer_length){	// if there is more to transmit
			spi_send(spi.buffer[spi.buffer_index]);
			spi.state = ST_TRANSMITTING;
		}
		else{
			spi.end(spi.header, spi.buffer, spi.buffer_index);
			spi.state = ST_IDLE;
		}
		break;
	default:
		spi.state = ST_IDLE;
		break;
	}
}

void spi_slave_init(spi_slave_start_ptr start_fcn, spi_slave_end_ptr end_fcn) {
	// Initialize Global Variables
	spi.start = start_fcn;
	spi.end = end_fcn;
	spi.buffer_index = 0;
	spi.buffer_length = 0;
	spi.state = ST_IDLE;
	// Initialize Hardware
	spi_enable_mosi();	// Make MISO (PB4) Output
	spi_enable();		// Enable SPI Hardare
}
