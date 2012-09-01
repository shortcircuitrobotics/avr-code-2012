/*
 * dogm.c
 *
 *  Created on: Jun 25, 2012
 *      Author: kevin
 */

#include <util/delay.h>
#include "dogm.h"

#define SETUP_DELAY 0

void dogm_send(uint8_t byte){
	SS_LCD_low();
	SPDR = byte;
	while(!(SPSR & (1<<SPIF)));
	SS_LCD_high();
}

void dogm_init(){
	// Enable SPI Master MOSI(PB5), SCK(PB7), SS_LCD(PB2), A0 (PD5), LCD_RST(PB4)
	DDRD |= (1<<PD5);
	DDRB |= (1<<PB2) | (1<<PB4) | (1<<PB5)|(1<<PB7); // Set MOSI and SCK output
	SPCR = (1<<SPE)|(1<<MSTR); // Enable SPI, Master, set clock rate fck/4
	SS_LCD_high();
	// Reset LCD
	RST_LCD_low();
	_delay_us(6);
	RST_LCD_high();
	_delay_us(10);

	// Send init commands
	dogm_send(0x40);	// Display start line 0
	dogm_send(0xA1);	// ADC reverse
	dogm_send(0xC0);	// Normal COM0-COM63
	dogm_send(0xA6);	// Display normal
	dogm_send(0xA2);	// Set bias 1/9 (Duty 1/65)
	dogm_send(0x2F);	// Booster, Regulator and Follower on
	dogm_send(0xF8);	// Set Internal Booster
	dogm_send(0x00);	// to 4x
	dogm_send(0x27);	// Set Contrast
	dogm_send(0x81);	//
	dogm_send(0x16);	//
	dogm_send(0xAC);	// Turn Indicator off
	dogm_send(0x00);	//
	dogm_send(0xA4);	// Turn all pixels on
	dogm_send(0xAF);	// Display on
}
