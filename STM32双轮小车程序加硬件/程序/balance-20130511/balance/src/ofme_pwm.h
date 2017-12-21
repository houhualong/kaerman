#ifndef __OFME_PWM_H__
#define __OFME_PWM_H__

#include "sys.h"
void pwm_init(void);
void hw_pwm_init(u16 arr,u16 psc);
void pwm_control(s16 moto1, s16 moto2);

#endif
