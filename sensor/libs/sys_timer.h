/*
 * sys_timer.h
 *
 *  Created on: Jul 1, 2012
 *      Author: kevin
 */

#ifndef SYS_TIMER_H_
#define SYS_TIMER_H_

#include <avr/io.h>

extern uint32_t system_time;

#define sys_timer_get() (system_time)

// Timer/Counter 2 (managed by motors.h) is the System Timer
void sys_timer_init();
void sys_timer_set_compass_timeout(uint16_t ms);
void sys_timer_init_motor_timer(uint8_t period_ms);


#endif /* SYS_TIMER_H_ */
