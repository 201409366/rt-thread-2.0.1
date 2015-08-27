#ifndef __MACHINE_H__
#define __MACHINE_H__
#include <rtthread.h>

void rt_hw_machine_init(void);
void rt_hw_setMathine1PWM(rt_uint8_t pwm);
void rt_hw_setMathine2PWM(rt_uint8_t pwm);
void rt_hw_setMathine3PWM(rt_uint8_t pwm);
void rt_hw_setMathine4PWM(rt_uint8_t pwm);
#endif /* __MACHINE_H__ */
