/*
 * tsop.c
 *
 *  Created on: 21.07.2012
 *      Author: christoph
 */


#include "tsop.h"
#include "adc.h"

uint16_t tsop_data[16];

uint8_t tsop_index_table[16]={
		13,
		14,
		4,
		0,
		3,
		15,
		1,
		2,
		12,
		11,
		10,
		9,
		8,
		5,
		7,
		6
};

static inline uint8_t get_index(uint8_t i){
	return tsop_index_table[i];
}

static struct{
	enum{
		IDLE = 0x00,
		WAITING_FOR_ADC = 0x01
	}state;
		uint8_t counter;
		uint16_t* array;
}tsop;



uint8_t tsops_run(void){
	switch(tsop.state){
	case IDLE:
		break;
	case WAITING_FOR_ADC:
		if(adc_complete){
			tsop.array[get_index(tsop.counter)] = adc_result;
			if(tsop.counter==15){
				PORTC &= ~(1<<PC1 | 1<<PC2 | 1<<PC3); 	//disable both MUX
				PORTD &= ~(1<<PD6 | 1<<PD7);			//set MUX-Select-PINs LOW
				tsop.counter==0;
				tsop.state = IDLE;
			}
			else{
				mux_select(tsop.counter+1);
				tsop.counter++;
				adc_start_conversion();
			}
		}
		break;
	}
	return tsop.state;

}

void adc_start_read(uint16_t*  data){
	mux_select(0);
	tsop.counter = 0;
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
