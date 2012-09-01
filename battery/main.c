/*
 * main.c
 *
 *  Created on: May 18, 2012
 *      Author: kevin
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

// Voltage Constants
#define V_MOTOR_OK 			910	// ~16.00
#define V_MOTOR_LOW 		852 // ~15.0V
#define V_MOTOR_OFF 		50
#define V_LOGIC_OK 			950	// ~4.00V
#define V_LOGIC_LOW 		887	// ~3.70


#define TURN_OFF_THRESHOLD 8

#define adc_complete !(ADCSRA & (1 << ADSC))
#define adc_start_conversion() { ADCSRA |= (1 << ADSC); }

#define power_off() PORTB |= (1<<PB2)
#define power_on() PORTB &= ~(1<<PB2);

#define led_all_off() { PORTB &= ~(0x03); PORTA &= ~(0x0C); }
#define led_3v7_ok_on() PORTA |= (1<<PA2)
#define led_3v7_low_on() PORTA |= (1<<PA3)
#define led_14v8_ok_on() PORTB |= (1<<PB1)
#define led_14v8_low_on() PORTB |= (1<<PB0)

// Disable Watchdog
// http://www.nongnu.org/avr-libc/user-manual/group__avr__watchdog.html
void get_mcusr(void) \
  __attribute__((naked)) \
  __attribute__((section(".init3")));
void get_mcusr(void)
{
  MCUSR = 0;
  wdt_disable();
}

typedef union{
	uint8_t b;
	struct{
		unsigned :2;
		unsigned v_motor_off: 1;
		unsigned v_motor_ok: 1;
		unsigned v_motor_low: 1;
		unsigned v_logic_ok: 1;
		unsigned v_logic_low: 1;
		unsigned turn_off: 1;
	};
} adc_data;

static inline void sleep_wdt();
static inline adc_data read_adc();
static inline void sleep_forever();

uint8_t turn_off_counter;
uint16_t adc_result;

void main(void){
	// PA2: 3.3V OK
	// PA3: 3.3V LOW
	// PB0: 14.8V OK
	// PB1: 14.8V LOW
	DDRA |= (1<<PA3) | (1<<PA2);
	DDRB |= (1<<PB1) | (1<<PB0);
	// PB2: FET
	DDRB |= (1<<PB2);
	power_on();
	// reset turn off counter
	turn_off_counter = 0;

	while(1){
		adc_data data = read_adc();
		led_all_off();
		if(data.turn_off){
			turn_off_counter++;
			if(turn_off_counter >= TURN_OFF_THRESHOLD){
				power_off();
				sleep_forever();
			}
		}
		else{
			turn_off_counter = 0;
		}
		// Motor LEDs
		if(data.v_motor_ok){
			led_14v8_ok_on();
		}
		else if(data.v_motor_low){
			led_14v8_low_on();
		}
		// Logic LEDs
		if(data.v_logic_ok){
			led_3v7_ok_on();
		}
		else if(data.v_logic_low){
			led_3v7_low_on();
		}
		// Sleep
		sleep_wdt();	// sleep using the watchdog timer
	}
}

static inline adc_data read_adc(){
	adc_data data;
	data.b = 0;
	//Set Sleep Mode
	set_sleep_mode(SLEEP_MODE_ADC);
	// PA0:  4.2V    3:1 Voltage Divider
	// PA1: 16.8V 15.7:1 Voltage Divider
	// Setup ADC
	ADMUX = (1<<REFS1);										// 1.1V Reference; PA0
	ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADPS2)| (1<<ADIE);	// ADC enabled; Prescaler: 16; start dummy conversion; interrupt enabled
	sei();			// enable interrupts
	sleep_mode();	// sleep until end of conversion
	// Measure Logic Voltage
	adc_start_conversion();
	sei();			// enable interrupts
	sleep_mode();	// sleep until end of conversion
	if(adc_result >= V_LOGIC_OK){
		data.v_logic_ok = true;
	}
	else if(adc_result >= V_LOGIC_LOW){
		data.v_logic_low = true;
	}
	else{
		data.turn_off = true;
		return data;
	}
	// Measure Motor Voltage
	ADMUX |= (1<<MUX0);	//PA1
	adc_start_conversion();
	sei();			// enable interrupts
	sleep_mode();	// sleep until end of conversion
	if(adc_result >= V_MOTOR_OK){
		data.v_motor_ok = true;
	}
	else if(adc_result >= V_MOTOR_LOW){
		data.v_motor_low = true;
	}
	else if(adc_result > V_MOTOR_OFF){
		data.turn_off = true;
	}
	else{
		data.v_motor_off = true;
	}
	// Turn ADC off
	ADCSRA = 0x00;
	return data;
}

ISR(ADC_vect){
	cli();				// disable interrupts
	adc_result = ADCW;	// save adc result
}

static inline void sleep_wdt(){
	wdt_reset();								// reset watch dog
	WDTCSR |= (1<<WDCE) | (1<<WDE);				// enable watchdog change
	WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP0);	// set watchdog
	sei();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);		// set deep sleep mode
	sleep_mode();

}

static inline void sleep_forever(){
	// Disable GPIOs
	DDRA = 0x00;
	PORTA = 0x00;
	DDRB = 0x00;
	PORTB = 0x00;
	// go to sleep
	cli();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_mode();
}

ISR(WDT_vect)	// watchdog interrupt
{
	cli();
	wdt_disable();	// only call this once
	sei();
}
