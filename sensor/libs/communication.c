#include <avr/io.h>

#define GET_BALL_POS 0xA1
#define GET_PROXIMITY 0xB2

uint8_t ball_pos_msb = 30;
uint8_t ball_pos_lsb = 90;
uint8_t distance = 100;

uint8_t prox_right = 100;
uint8_t prox_back = 100;
uint8_t prox_left = 100;
uint8_t prox_front = 100;

uint8_t send_data(uint8_t header,uint8_t* spi_buffer){
	uint8_t bytes_written = 0;
	switch(header){
	case GET_BALL_POS:
		spi_buffer[0] = ball_pos_msb;
		spi_buffer[1] = ball_pos_lsb;
		spi_buffer[2] = distance;
		bytes_written = 3;
		break;
	case GET_PROXIMITY:
		spi_buffer[0] = prox_right;
		spi_buffer[1] = prox_back;
		spi_buffer[2] = prox_left;
		spi_buffer[3] = prox_front;
		bytes_written = 4;
		break;
	default:
		bytes_written = 0;
		break;
	}
	return bytes_written;
}

void spi_end(uint8_t header,uint8_t* spi_data_start,uint8_t received_bytes){

}
