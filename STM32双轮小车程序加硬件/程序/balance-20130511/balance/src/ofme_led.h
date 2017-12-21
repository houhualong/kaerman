#ifndef __OFME_LED_H__
#define __OFME_LED_H__

#include <stm32f10x_lib.h>

void led_init(void);

#define LED0_OFF()	GPIOA->BSRR = 1<<8
#define LED1_OFF()	GPIOD->BSRR = 1<<2
#define LED0_ON()	GPIOA->BRR  = 1<<8
#define LED1_ON()	GPIOD->BRR  = 1<<2



#endif

