#ifndef TRIG_INT_H_
#define TRIG_INT_H_

#include <avr/io.h>

#define TRIG_INT_SIN_90 255
#define abs_(a)		(((a) < 0) ? -(a) : (a))

extern int16_t sin_(int16_t angle);
extern int16_t cos_(int16_t angle);

#endif /* TRIG_INT_H_ */
