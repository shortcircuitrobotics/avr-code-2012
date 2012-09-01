/*
 * dribbler.c
 *
 *  Created on: 22.08.2012
 *      Author: christoph
 */


#include "avr/io.h"
#include "dribbler.h"

void dribbler_init(){
	TCCR2A = (1<<COM2A1 | 1<<WGM20 |1<<WGM21); //fast PWM on OC2A
	TCCR2B = (1<<CS21); //start timer prescaler: 8
	OCR2A = 0x00;	//duty cycle
	DDRA |= (1<<PA0 | 1<<PA1);	//set FWD_REW und BREAK output
	DDRD |= (1<<PD7);		//set PWM pin output

}
