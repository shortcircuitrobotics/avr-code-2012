/*
 * dribbler.h
 *
 *  Created on: 22.08.2012
 *      Author: christoph
 */

#include "avr/io.h"

#define break_off() 	PORTA |= (1<<PA1);
#define set_fwd()   	PORTA |= (1<<PA0);

void dribbler_init();


