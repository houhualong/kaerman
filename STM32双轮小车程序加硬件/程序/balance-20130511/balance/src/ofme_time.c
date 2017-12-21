#include "ofme_time.h"

unsigned int sys_tick = 0;

void hw_tick_start()
{
	unsigned int cnts;

	cnts = (unsigned int)9000000 / RT_TICK_PER_SECOND;

	SysTick->CTRL&=0xfffffffb;//bit2清空,选择外部时钟，也即HCLK/8
	SysTick->LOAD=cnts-1;
	SysTick->CTRL=0x01 ;          //开始倒数   不产生中断

	SysTick->CTRL = 0x03;	// 启动systick并产生中断
}

extern void sys_tick_proc(void);
void hw_delay_us(unsigned int us)
{
	unsigned int start ,target,cur;

	while(us > 500)	   // 防止us超过systick周期。（假设systick周期超过500us）
	{
		hw_delay_us(500);
		us -= 500;
	}

	start = SysTick->VAL;
	target = (start-us*9);
	
	if(start<target)	// 溢出，调整target的值
	{
		target += (9000000 / RT_TICK_PER_SECOND);
		do
		{
			cur = SysTick->VAL;
		}while(cur<=start || cur>target);  // target不能等于start，否则死循环。也即us不能等于systick周期
	}
	else   // target <= start
	{
		do
		{
			cur = SysTick->VAL;
		}while(target<cur&&cur<=start);	  // 假设target为一个很小的值，则cur有可能在循环周期内溢出而大于target，故须加上cur<=start的条件
	}
}

void hw_delay_ms(unsigned int ms)
{
	while(ms-- > 0)
	{
		hw_delay_us(500);
		hw_delay_us(500);
	}
}

void SysTick_Handler(void)
{
	sys_tick++;
	sys_tick_proc();
}

void hw_tsc_init()	// TSC-timestamp counter
{
	unsigned int cnts;

	cnts = (unsigned int)90000000 / RT_TICK_PER_SECOND;

	SysTick->CTRL&=0xfffffffb;//bit2清空,选择外部时钟，也即HCLK/8
	SysTick->LOAD=cnts-1;
	SysTick->VAL = 0; // write to clean
	SysTick->CTRL=0x01 ;          //开始倒数   不产生中断
}


u32 hw_time_get()
{
	u32 t1, t2, tus;
	t1 = sys_tick;
	tus = (10000-1)-(SysTick->VAL/9);
	t2 = sys_tick;
	
	if(t1==t2)	return t1*10000+tus;
	else return t2*10000;

}

u32 hw_interval_get(u32 us1,u32 us2)
{
		return us2-us1;
}



//u32 hw_time_get()
//{
//	return (u32)(SysTick->VAL/9);
//}
//
//u32 hw_interval_get(u32 us1,u32 us2)
//{
//	if(us2<=us1) return us1-us2;
//	else
//	{
//		us1 += 100000;	// 10000us == 10ms;
//		return us1-us2;
//	}
//}
//
//
//
