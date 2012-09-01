/*
 * sensor_com.h
 *
 *  Created on: 10.08.2012
 *      Author: christoph
 */

#ifndef SENSOR_COM_H_
#define SENSOR_COM_H_

void get_ball_pos(uint16_t* angle, uint8_t* distance);
void get_proximity(uint8_t* right, uint8_t* back, uint8_t* left, uint8_t* front);


#endif /* SENSOR_COM_H_ */
