/*
 * spi_master.h
 *
 *  Created on: 07.08.2012
 *      Author: christoph
 */

#ifndef SPI_MASTER_H_
#define SPI_MASTER_H_


#define SENSOR_SS 		0
#define MOTOR_SS 		1
#define MOUSESENSOR_SS 2
#define LCD_SS 			3
#define CPLD_SS 		4


void spi_master_init();
uint8_t spi_send_receive(uint8_t send_data, uint8_t* received_data, uint8_t chipselect);


#endif /* SPI_MASTER_H_ */
