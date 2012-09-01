/*
 * srf05.c
 *
 *  Created on: Feb 3, 2011
 *      Author: kevin
 */


#include "srf05.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>



/*
 * Timer used: 4
 *
 * Pins used:
 * 		Trigger  left: PD4
 * 		Echo     left: PC5 (PCINT13)
 * 		Trigger right: PD2
 * 		Echo    right: PB1 (PCINT1)
 * 		Trigger front: PD3
 * 		Echo    front: PB0 (PCINT0)
 * 		Trigger  back: PD5
 * 		Echo     back: PC4 (PCINT12)
 */

static struct{
	struct{
		unsigned pj5_high :1;
		unsigned pj5_low :1;
		unsigned pk7_high :1;
		unsigned pk7_low :1;
		unsigned pd2_high :1;
		unsigned pd2_low :1;
		unsigned pd3_high :1;
		unsigned pd3_low :1;
	};
} srf05;


#define enable_PCINT1() 		PCICR |= (1<<PCIE1)
#define disable_PCINT1() 		PCICR &= ~(1<<PCIE1)
#define enable_PCINT2() 		PCICR |= (1<<PCIE2)
#define disable_PCINT2() 		PCICR &= ~(1<<PCIE2)
//#define enable_INT2()			{ EIMSK |= (1<<INT2); EIFR  |= (1<<INTF2); }
#define disable_INT2()			EIMSK &= ~(1<<INT2)
//#define enable_INT3()			{ EIMSK |= (1<<INT3); EIFR  |= (1<<INTF3); }
#define disable_INT3()			EIMSK &= ~(1<<INT3)
#define enable_TIMER4_OVF()		TIMSK4 |= (1<<TOIE4)
#define disable_TIMER4_OVF()	TIMSK4 &= ~(1<<TOIE4)
#define stop_timer4()			{ TCCR4B &= ~(0x07); TCCR4A &= ~(1<<WGM41); }
#define start_timer4_skl_8()	TCCR4B |= (1<<CS41)

#define SRF05_PULSE_LENGTH 9
#define SRF05_ERROR 0xFFFF


void srf05_init(void){
	disable_INT2();
	disable_INT3();
	EICRA |= (0<<ISC21) | (1<<ISC20);	// set INT2 (PD2) to trigger
	EICRA |= (0<<ISC31) | (1<<ISC30);	// set INT3 (PD3) to trigger
}

void srf05_start_front(void){
	srf05.pd2_low = false;

	// send pulse
	set_bit(DDRD, 2);		// set PD2 output
	set_bit(PORTD, 2);
	_delay_us(SRF05_PULSE_LENGTH);
	clr_bit(PORTD, 2);
	clr_bit(DDRD, 2);			// set PD2 input
	EIFR  |= (1<<INTF2); 	// clear interrupt flag by writing a logical 1
	EIMSK |= (1<<INT2);		// enable interrupt
}

void srf05_start_back(void){
	srf05.pd3_low = false;

	// send pulse
	set_bit(DDRD, 3);		// set PD3 output
	set_bit(PORTD, 3);
	_delay_us(SRF05_PULSE_LENGTH);
	clr_bit(PORTD, 3);
	clr_bit(DDRD, 3);			// set PD3 input
	EIFR  |= (1<<INTF3); 	// clear interrupt flag by writing a logical 1
	EIMSK |= (1<<INT3);		// enable interrupt
}


void srf05_start_right(void){
	srf05.pk7_low = false;

	// send pulse
	set_bit(DDRK, 7);		// set PK7 output
	set_bit(PORTK, 7);
	_delay_us(SRF05_PULSE_LENGTH);
	clr_bit(PORTK, 7);
	clr_bit(DDRK, 7);			// set PK7 input
	set_bit(PCMSK2, PCINT23); 	// enable external interrupt on PK7
	enable_PCINT2();			// enable interrupt for PK7
}

void srf05_start_left(void){
	srf05.pj5_low = false;

	// send pulse
	set_bit(DDRJ, 5);		// set PJ5 output
	set_bit(PORTJ, 5);
	_delay_us(SRF05_PULSE_LENGTH);
	clr_bit(PORTJ, 5);
	clr_bit(DDRJ, 5);			// set PJ5 input
	set_bit(PCMSK1, PCINT14); 	// enable external interrupt on PJ5
	enable_PCINT1();			// enable interrupt for PJ5
}


void static inline srf05_done(){
	disable_TIMER4_OVF();	// disable timer overflow
	stop_timer4();
	if(srf05.pj5_low && srf05.pk7_low && srf05.pd2_low && srf05.pd3_low){
		sens.srf_save = true; // save srf data
	}
}


/****Interrupts***/

#define get_cm() (TCNT4) / (2*58)

// PJ5 (left)
ISR(PCINT1_vect){			// external interrupt 1 (PJ5)
	//deb("PCINT1_vect");
	if(srf05.pj5_high){
		uint8_t cm;
		srf05.pj5_high = false;		// switched from HIGH
		srf05.pj5_low = true;		// to LOW
		disable_PCINT1();				// disable interrupt
		clr_bit(PCMSK1, PCINT14); 		// disable external interrupt on PJ5
		cm = get_cm();	// convert to cm
		srf_tmp.left = cm - SRF05_LEFT_OFFSET;
		srf05_done();
	}
	else{
		srf05.pj5_high = true;
		enable_TIMER4_OVF();	// start timer
		TCNT4 = 0;
		start_timer4_skl_8();
	}
}

// PK7 (right)
ISR(PCINT2_vect){			// external interrupt 2 (PK7)
	//deb("PCINT2_vect");
	if(srf05.pk7_high){
		uint16_t cm;
		srf05.pk7_high = false;		// switched from HIGH
		srf05.pk7_low = true;		// to LOW
		disable_PCINT2();				// disable interrupt
		clr_bit(PCMSK2, PCINT23); 		// disable external interrupt on PK7
		cm = get_cm();	// convert to cm
		srf_tmp.right = cm - SRF05_RIGHT_OFFSET;
		srf05_done();
	}
	else{
		srf05.pk7_high = true;
		enable_TIMER4_OVF();	// start timer
		TCNT4 = 0;
		start_timer4_skl_8();
	}
}

// PD2 (front)
ISR(INT2_vect){			// interrupt 2 (PD2)
	//deb("INT2_VECT");
	if(srf05.pd2_high){
		uint16_t cm;
		srf05.pd2_high = false;		// switched from HIGH
		srf05.pd2_low = true;		// to LOW
		disable_INT2();				// disable interrupt
		cm = get_cm();	// convert to cm
		srf_tmp.front = cm - SRF05_FRONT_OFFSET;
		srf05_done();
	}
	else{
		srf05.pd2_high = true;
		enable_TIMER4_OVF();	// start timer
		TCNT4 = 0;
		start_timer4_skl_8();
	}
}

// PD3 (back)
ISR(INT3_vect){			// interrupt 3 (PD3)
	//deb("INT3_VECT");
	if(srf05.pd3_high){
		uint16_t cm;
		srf05.pd3_high = false;		// switched from HIGH
		srf05.pd3_low = true;		// to LOW
		disable_INT3();				// disable interrupt
		cm = get_cm();	// convert to cm
		srf_tmp.back = cm - SRF05_BACK_OFFSET;
		srf05_done();
	}
	else{
		srf05.pd3_high = true;
		enable_TIMER4_OVF();	// start timer
		TCNT4 = 0;
		start_timer4_skl_8();
	}
}

ISR(TIMER4_OVF_vect){		// timer 4 overflow
	// ERROR !!!
	deb("TIMER 4 OVERFLOW (SRF05)");
	disable_PCINT1();				// disable interrupt
	clr_bit(PCMSK1, PCINT14); 		// disable external interrupt on PJ5
	disable_PCINT2();				// disable interrupt
	clr_bit(PCMSK2, PCINT23); 		// disable external interrupt on PK7
	disable_INT2();					// disable interrupt
	disable_INT3();					// disable interrupt
	disable_TIMER4_OVF();			// disable timer overflow
	stop_timer4();
}

