/*
 * tsop.c
 *
 *  Created on: 21.07.2012
 *      Author: christoph
 */


#include "tsop.h"
#include "adc.h"

static struct{
	enum{
		IDLE = 0x00,
		WAITING_FOR_ADC = 0x01
	}state;
		uint8_t last_read;
		uint16_t* array;
}tsop;



uint8_t tsops_run(void){
	switch(tsop.state){
	case IDLE:
		break;
	case WAITING_FOR_ADC:
		if(adc_complete){
			*tsop.array = adc_result;
			if(tsop.last_read==15){
				PORTC &= ~(1<<PC1 | 1<<PC2 | 1<<PC3); 	//disable both MUX
				PORTD &= ~(1<<PD6 | 1<<PD7);			//set MUX-Select-PINs LOW
				tsop.state = IDLE;
			}
			else{
				tsop.array++;
				mux_select(tsop.last_read+1);
				tsop.last_read++;
				adc_start_conversion();
			}
		}
		break;
	}
	return tsop.state;

}

void adc_start_read(uint16_t*  data){
	mux_select(0);
	tsop.last_read = 0;
	adc_start_conversion();
	tsop.array = data;
	tsop.state = WAITING_FOR_ADC;
}


char* tsop_str [16]={
		"TSOP 13",
		"TSOP 14",
		"TSOP 04",
		"TSOP 00",
		"TSOP 03",
		"TSOP 15",
		"TSOP 01",
		"TSOP 02",
		"TSOP 12",
		"TSOP 11",
		"TSOP 10",
		"TSOP 09",
		"TSOP 08",
		"TSOP 05",
		"TSOP 07",
		"TSOP 06"

};
