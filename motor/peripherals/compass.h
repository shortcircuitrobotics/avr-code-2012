/*
 * compass.h
 *
 *  Created on: Jul 1, 2012
 *      Author: kevin
 */

#ifndef COMPASS_H_
#define COMPASS_H_

#include <avr/io.h>
#include <stdbool.h>

#define SCL_CLOCK  100000L // i2c clock frequency
#define HMC6352_ADDRESS 0x42
#define HMC6352_READ_EEPROM 0x72
#define HMC6352_READ_HEADING 0x41

void compass_init();
void compass_start();
void compass_run();
bool compass_new_heading();		// returns true if there is new heading information available
uint16_t compass_read_heading();

#endif /* COMPASS_H_ */
