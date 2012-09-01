/*
 * srf05.h
 *
 *  Created on: Feb 3, 2011
 *      Author: kevin
 */

#ifndef SRF05_H_
#define SRF05_H_

#define SRF05_FRONT_OFFSET 4
#define SRF05_BACK_OFFSET 2
#define SRF05_LEFT_OFFSET 2
#define SRF05_RIGHT_OFFSET 2

void srf05_start_front(void);
void srf05_start_back(void);
void srf05_start_left(void);
void srf05_start_right(void);


void srf05_init(void);



#endif /* SRF05_H_ */
