#include "ofme_time.h"

unsigned int sys_tick = 0;

void hw_tick_start()
{
	unsigned int cnts;

	cnts = (unsigned int)9000000 / RT_TICK_PER_SECOND;

	SysTick->CTRL&=0xfffffffb;//bit2���,ѡ���ⲿʱ�ӣ�Ҳ��HCLK/8
	SysTick->LOAD=cnts-1;
	SysTick->CTRL=0x01 ;          //��ʼ����   �������ж�

	SysTick->CTRL = 0x03;	// ����systick�������ж�
}

extern void sys_tick_proc(void);
void hw_delay_us(unsigned int us)
{
	unsigned int start ,target,cur;

	while(us > 500)	   // ��ֹus����systick���ڡ�������systick���ڳ���500us��
	{
		hw_delay_us(500);
		us -= 500;
	}

	start = SysTick->VAL;
	target = (start-us*9);
	
	if(start<target)	// ���������target��ֵ
	{
		target += (9000000 / RT_TICK_PER_SECOND);
		do
		{
			cur = SysTick->VAL;
		}while(cur<=start || cur>target);  // target���ܵ���start��������ѭ����Ҳ��us���ܵ���systick����
	}
	else   // target <= start
	{
		do
		{
			cur = SysTick->VAL;
		}while(target<cur&&cur<=start);	  // ����targetΪһ����С��ֵ����cur�п�����ѭ�����������������target���������cur<=start������
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

	SysTick->CTRL&=0xfffffffb;//bit2���,ѡ���ⲿʱ�ӣ�Ҳ��HCLK/8
	SysTick->LOAD=cnts-1;
	SysTick->VAL = 0; // write to clean
	SysTick->CTRL=0x01 ;          //��ʼ����   �������ж�
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
