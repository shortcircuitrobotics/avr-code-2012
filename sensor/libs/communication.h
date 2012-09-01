/*
 * communication.h
 *
 *  Created on: 07.08.2012
 *      Author: christoph
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

uint8_t send_data(uint8_t header,uint8_t* spi_buffer);
void spi_end(uint8_t header,uint8_t* spi_data_start,uint8_t received_bytes);

#endif /* COMMUNICATION_H_ */
