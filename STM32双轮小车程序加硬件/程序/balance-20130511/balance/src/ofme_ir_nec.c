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

// ���յ�����ֵ
u32 ir_data;
// <0: ir_data��Ч; 0:ir_data��Ч�����Ѿ����������>0: ������������>0���ⲿ������Խ���-1������ʾ��ȡ����
int	ir_repeat = -1;
// 0: OK; >0: error count; �ⲿ����ɶ�ȡ����ֵ�˽����޸����źŻ�����debug
int ir_err_cnt = 0;

void hw_ir_init(void)
{

//��ʼ������������ŵ�����
//�����ж�,��ӳ�� 
	RCC->APB2ENR|=1<<4;       //PCʱ��ʹ��		  
	GPIOC->CRL&=0XFFFFFF0F;
	GPIOC->CRL|=0X00000080;	//PC1����	 
	GPIOC->ODR|=1<<1;		//PC.1����      
	Ex_NVIC_Config(GPIO_C,1,FTIR);//��line1ӳ�䵽PC.1���½��ش���.
	MY_NVIC_Init(2,1,EXTI1_IRQChannel,2);

}
// һ�廯�������ͷֻ��ͨ��38kHz���ҵ��ز���������&���ܣ�����ת��ΪTTL�͵�ƽ
// �½����ж�
void EXTI1_IRQHandler(void)
//void ir_nec_receive(void)
{
//	step(<=-1����ʾ�ظ�֡����; 0����ʾ����֡�Ŀ�ͷ��32����ʾ����֮֡��ļ����>=33����ʾ�ظ�֡�Ŀ�ͷ)
	static int step=-1;
	static int time1=0;
	int	time2;
	int interval;

	time2 = hw_time_get();
	interval = hw_interval_get(time1,time2);
	time1 = time2;

	if(interval>NEC_REPEAT_DELAY_MAX)//������֮��������
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
		ir_repeat=-1; // ��ʾir_data��Ч
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

// ֻ�г���Ϊ0��1���������ִ�е�����
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
	else if(step>=0) // ���ݽ��ճ���
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
