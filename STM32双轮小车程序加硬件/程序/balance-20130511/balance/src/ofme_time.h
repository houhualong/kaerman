#ifndef __OFME_TIME_H__
#define __OFME_TIME_H__

//#include "stm32f10x.h"
#include "sys.h"


#define RT_TICK_PER_SECOND	(100)
#define HW_TICK_PER_SECOND RT_TICK_PER_SECOND
#define HW_TICK_GET()	100	
#define HW_CLOCK_US() (unsigned)(SysTick->VAL)/9
#define HW_TIME_CYCLE (1000000ul/RT_TICK_PER_SECOND)

void hw_tick_start(void);
extern void sys_tick_proc(void);
void hw_delay_us(unsigned int us);
void hw_delay_ms(unsigned int ms);
void SysTick_Handler(void);
void hw_tsc_init(void);	// TSC-timestamp counter
u32 hw_time_get(void);
u32 hw_interval_get(u32 us1,u32 us2);

#endif

