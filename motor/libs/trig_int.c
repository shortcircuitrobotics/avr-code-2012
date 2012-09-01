/*
 * trig_int.c
 *
 *  Created on: Mar 12, 2011
 *      Author: kevin
 */

#include "trig_int.h"

static uint8_t sin_0_90_table[91] =
{
		0, 4, 9, 13, 18, 22, 27, 31, 35, 40, 44, 49, 53, 57, 62, 66, 70, 75, 79, 83, 87, 91, 96,
		100, 104, 108, 112, 116, 120, 124, 127, 131, 135, 139, 143, 146, 150, 153, 157, 160, 164,
		167, 171, 174, 177, 180, 183, 186, 190, 192, 195, 198, 201, 204, 206, 209, 211, 214, 216,
		219, 221, 223, 225, 227, 229, 231, 233, 235, 236, 238, 240, 241, 243, 244, 245, 246, 247,
		248, 249, 250, 251, 252, 253, 253, 254, 254, 254, 255, 255, 255, 255
};


int16_t sin_(int16_t angle){
	uint8_t quadrant;
	quadrant = (angle / 90) % 4;
	angle %= 90;
	switch(quadrant){
	case 0:
        return sin_0_90_table[angle];
	case 1:
        return sin_0_90_table[90-angle];
	case 2:
        return -sin_0_90_table[angle];
	case 3:
        return -sin_0_90_table[90-angle];
	}
	return 0;
}

int16_t cos_(int16_t angle){
    return sin_(angle+90);
}
