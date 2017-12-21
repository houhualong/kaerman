#include "ofme_ir_nec.h"
#include "ofme_time.h"

#define NEC_HEAD_PLUSE_MAX		(9000+900)
#define NEC_HEAD_PLUSE_MIN		(9000-630)
#define NEC_HEAD_SPACE_MAX		(4500+450)
#define NEC_HEAD_SPACE_MIN		(4500-315)
#define NEC_HEAD_MAX			(NEC_HEAD_PLUSE_MAX+NEC_HEAD_SPACE_MAX)
#define NEC_HEAD_MIN			(NEC_HEAD_PLUSE_MIN+NEC_HEAD_SPACE_MIN)
#define NEC_DATA_PLUSE_MAX		(560+56)
#define NEC_DATA_PLUSE_MIN		(560-56)
#define NEC_LOG0_SPACE_MAX		(560+56)
#define NEC_LOG0_SPACE_MIN		(560-56)
#define NEC_LOG0_MAX			(NEC_DATA_PLUSE_MAX+NEC_LOG0_SPACE_MAX)
#define NEC_LOG0_MIN			(NEC_DATA_PLUSE_MIN+NEC_LOG0_SPACE_MIN)
#define NEC_LOG1_SPACE_MAX		(1680+168)
#define NEC_LOG1_SPACE_MIN		(1680-168)
#define NEC_LOG1_MAX			(NEC_DATA_PLUSE_MAX+NEC_LOG1_SPACE_MAX)
#define NEC_LOG1_MIN			(NEC_DATA_PLUSE_MIN+NEC_LOG1_SPACE_MIN)
#define NEC_REPEAT_PLUSE_MAX	(9000+630)
#define NEC_REPEAT_PLUSE_MIN	(9000-900)
#define NEC_REPEAT_SPACE_MAX	(2500+175)
#define NEC_REPEAT_SPACE_MIN	(2500-250)
#define NEC_REPEAT_DELAY_MAX	(97940+9794+NEC_DATA_PLUSE_MAX)
#define NEC_REPEAT_DELAY_MIN	(97940-9794+NEC_DATA_PLUSE_MIN)
#define NEC_REPEAT_MAX			(NEC_REPEAT_PLUSE_MAX+NEC_REPEAT_SPACE_MAX)
#define NEC_REPEAT_MIN			(NEC_REPEAT_PLUSE_MIN+NEC_REPEAT_SPACE_MIN)

#define IR_INT_CLR()			EXTI->PR = 1<<1

#define IR_NEC_DEBUG

// 接收到的数值
u32 ir_data;
// <0: ir_data无效; 0:ir_data有效，但已经被处理过；>0: 连发次数；当>0，外部程序可以进行-1操作表示读取数据
int	ir_repeat = -1;
// 0: OK; >0: error count; 外部程序可读取此数值了解有无干扰信号或用于debug
int ir_err_cnt = 0;

void hw_ir_init(void)
{

//初始化红外接收引脚的设置
//开启中断,并映射 
	RCC->APB2ENR|=1<<4;       //PC时钟使能		  
	GPIOC->CRL&=0XFFFFFF0F;
	GPIOC->CRL|=0X00000080;	//PC1输入	 
	GPIOC->ODR|=1<<1;		//PC.1上拉      
	Ex_NVIC_Config(GPIO_C,1,FTIR);//将line1映射到PC.1，下降沿触发.
	MY_NVIC_Init(2,1,EXTI1_IRQChannel,2);

}
// 一体化红外接收头只能通过38kHz左右的载波（抗干扰&节能），并转化为TTL低电平
// 下降沿中断
void EXTI1_IRQHandler(void)
//void ir_nec_receive(void)
{
//	step(<=-1：表示重复帧结束; 0：表示数据帧的开头；32：表示连续帧之间的间隔；>=33：表示重复帧的开头)
	static int step=-1;
	static int time1=0;
	int	time2;
	int interval;

	time2 = hw_time_get();
	interval = hw_interval_get(time1,time2);
	time1 = time2;

	if(interval>NEC_REPEAT_DELAY_MAX)//连发码之间的最大间隔
	{
		step = -1;
		goto err;
	}
	else if(interval>NEC_HEAD_MAX)
	{
		goto err;
	}
	else if(interval>NEC_HEAD_MIN)
	{
		ir_repeat=-1; // 表示ir_data无效
		ir_data = 0;
		step = 0;
#ifdef IR_NEC_DEBUG
		putchar('[');
#endif
		IR_INT_CLR();
		return;
	}
	else if(interval>NEC_REPEAT_MAX)
	{
		goto err;
	}
	else if(interval>NEC_REPEAT_MIN)
	{
		if(step != 33) goto err;
		step = 32;
		ir_repeat++;
#ifdef IR_NEC_DEBUG
		putchar('-');
		putchar('R');
		printf("-%d*%d.", (ir_data>>16)&0x0FF, ir_repeat);
#endif
		IR_INT_CLR();
		return;
	}
	else if(interval>NEC_LOG1_MAX)
	{
		goto err;
	}
	else if(interval>NEC_LOG1_MIN)
	{
		goto decode;
	}
	else if(interval>NEC_LOG0_MAX)
	{
		goto err;
	}
	else if(interval>NEC_LOG0_MIN)
	{
		goto decode;
	}
	else
	{
		goto err;
	}

// 只有长度为0或1的脉冲才能执行到这里
decode:
	if(step<0 || step>=32) goto err;
	ir_data>>= 1;
	if(interval>NEC_LOG1_MIN)
	{
		ir_data |= 0x80000000UL;
	}
	step++;
	if(step==32)
	{
		ir_repeat = 1;
#ifdef IR_NEC_DEBUG
		putchar(']');
		printf("-%d*%d.", (ir_data>>16)&0x0FF, ir_repeat);
#endif
	}
	IR_INT_CLR();
	return;

err:
#ifdef IR_NEC_DEBUG
	putchar('\r');
	putchar('\n');
#endif
	if(step == 32)
	{
		step = 33;
#ifdef IR_NEC_DEBUG
		putchar('R');
#endif
	}
	else if(step>=0) // 数据接收出错
	{
		ir_err_cnt++;
		step = -1;
#ifdef IR_NEC_DEBUG
		putchar('E');
#endif
	}
	else
	{
#ifdef IR_NEC_DEBUG
		putchar('S');
#endif
	}

	IR_INT_CLR();
	return;
}
