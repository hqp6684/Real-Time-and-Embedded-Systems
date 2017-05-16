#ifndef __TIMER_H
#define __TIMER_H

#include "stm32l476xx.h"
#include "SysClock.h"

#define PWM0 (0)
#define PWM1 (1)

void pwmstart(uint32_t servo_null);
void changePWM(uint8_t pwm_num, uint32_t pwmrange);

#endif
