/*
 * i2c_async.h
 *
 *  Created on: Jan 20, 2011
 *      Author: kevin
 */

#ifndef I2C_ASYNC_H_
#define I2C_ASYNC_H_


#include <util/twi.h>
#include <stdbool.h>

#define I2C_WRITE 0
#define I2C_READ 1

/***********************Functions****************/
//public:
#define i2c_async_init(scl_clk) i2c_async_init_(((F_CPU/scl_clk)-16)/2)
void i2c_async_init_(uint8_t twbr_);			// initializes I2C
uint8_t i2c_async_run(void);					// run State Machine; returns 0 when state=IDLE
void i2c_async_start(uint8_t address);		// starts the i2c connection
void i2c_async_start_wait(uint8_t address);	// starts the i2c connection w/ wait
void i2c_async_stop(void);					// stops the i2c connection
void i2c_async_read_ack(void);				// starts read with acknowledge
void i2c_async_read_nak(void);				// starts read without acknowledge
void i2c_async_read_word(void);				// starts reading two uint8_t (1. MSB, 2.LSB)
void i2c_async_read_array(uint16_t* a, uint8_t length, uint8_t bit_mask);	// starts reading 16bit values into array
void i2c_async_read_array_omit(uint16_t* a, uint8_t length, uint8_t bit_mask, uint8_t omit); // starts reading 16bit values into array, omits the value specified
void i2c_async_write(uint8_t b);		// starts write
void i2c_async_write_2(uint8_t b1, uint8_t b2);		//starts writing two commands
uint8_t i2c_async_get_byte(void);		// get the i2c byte value
uint16_t i2c_async_get_word(void);		// get the i2c word value
uint8_t i2c_async_error(void);			// returns 1 if an error occurred; 0 if not

void i2c_async_disable(void);	// stops the i2c hw




#endif /* I2C_ASYNC_H_ */
