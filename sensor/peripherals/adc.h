/*
 * adc_async.h
 *
 *  Created on: Jan 28, 2011
 *      Author: kevin
 */

#ifndef ADC_ASYNC_H_
#define ADC_ASYNC_H_

#include <avr/io.h>

#define adc_complete !(ADCSRA & (1 << ADSC))
#define adc_start_conversion() { ADCSRA |= (1 << ADSC); }
#define adc_result ADCW

void adc_init(void);			// initializes ADC
void adc_set_mux(uint8_t mux);

#endif /* ADC_ASYNC_H_ */
