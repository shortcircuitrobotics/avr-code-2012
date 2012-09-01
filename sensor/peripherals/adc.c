/*
 * adc_async.c
 *
 *  Created on: Jan 28, 2011
 *      Author: kevin
 *
 *
 *
 *
 *  pins used
 *  PC0 MUX_OUT
 *  PC1-PC3 MUX_SEL
 *  PD6-PD7 MUX-EN
 */

#include "adc.h"
#include <util/delay.h>

uint8_t last_MUX2;


#define MUX1_disable() PORTD &= ~(1<<PD6)
#define MUX2_disable() PORTD &= ~(1<<PD7)
#define MUX1_enable() PORTD |= (1<<PD6)
#define MUX2_enable() PORTD |= (1<<PD7)

/*
 * initializes ADC and Analog-MUX
 */
void adc_init(void){
	DDRC |= 0xE;
	DDRD |= (1<<PD6 | 1<<PD7); //set MUX-EN Pins
	ADCSRA = (1<<ADEN) | 0x7; 	// ADC enabled; prescaler 128 = 2⁷
	ADMUX = 0;			// ADC-Ref: AREF pin
	ADCSRA |= (1 << ADSC); while(ADCSRA & (1 << ADSC)); // one read to init
}

/*
 * selects between the inputs of both MUX
 */

void mux_select(uint8_t mux){
	if(mux<8){
		if(last_MUX2){
			MUX2_disable();
			_delay_ms(1);
			MUX1_enable();
		}
		PORTC &= 0xF1; //set MUX-SEL-PINs LOW
		PORTC |= (mux<<1); //set MUX-SEL-PINs
		last_MUX2=0;
	}
	else{
		if(!last_MUX2){
			MUX1_disable();
			_delay_ms(1);
			MUX2_enable();
		}
		PORTC &= 0xF1; //set MUX-SEL-PINs LOW
		PORTC |= ((mux-8)<<1); //set MUX-SEL-PINs
		last_MUX2=1;
	}
}
