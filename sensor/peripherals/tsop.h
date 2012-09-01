/*
 * tsop.h
 *
 *  Created on: 21.07.2012
 *      Author: christoph
 */

#ifndef TSOP_H_
#define TSOP_H_

#include <avr/io.h>

#define TSOP_00 3
#define TSOP_01 6
#define TSOP_02 7
#define TSOP_03 4
#define TSOP_04 2
#define TSOP_05 13
#define TSOP_06 15
#define TSOP_07 14
#define TSOP_08 12
#define TSOP_09 11
#define TSOP_10 10
#define TSOP_11 9
#define TSOP_12 8
#define TSOP_13 0
#define TSOP_14 1
#define TSOP_15 5

extern char* tsop_str [16];


uint8_t tsops_run(void);
void adc_start_read(uint16_t* data);


#endif /* TSOP_H_ */
