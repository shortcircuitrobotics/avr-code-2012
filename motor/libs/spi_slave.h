/*
 * spi.h
 *
 *  Created on: Jun 26, 2012
 *      Author: kevin
 *
 *      This file (together with spi_slave.c) contains a library written for
 *      atMega88 (should be easily portable to other avr devices). It allows for
 *      the controller to be used as a SPI Slave.
 *      Please Note: This Library only works if the data send via SPI is only dependent
 *      on the header byte received. It does not work if the data sent depends on the
 *      data just received.
 */

#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

#define SPI_SLAVE_BUFFER_LENGTH 16


typedef uint8_t (*spi_slave_start_ptr)(uint8_t,uint8_t*); 	// return: number of bytes written; args: header byte, ptr to spi buffer
typedef void (*spi_slave_end_ptr)(uint8_t,uint8_t*,uint8_t); 		// args: ptr to spi buffer, number of bytes written
void spi_slave_init(spi_slave_start_ptr start_fcn, spi_slave_end_ptr end_fcn);

#endif /* SPI_H_ */
