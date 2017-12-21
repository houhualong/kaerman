
#include "ofme_pwm.h"

void pwm_init(void)
{
	RCC->APB2ENR|=1<<4;    //PC时钟使能	   
	RCC->APB2ENR|=1<<2;    //PA时钟使能	   
	RCC->APB2ENR|=1<<0;    //开启辅助时钟							  
	GPIOC->CRL&=0XFFFF0000;//PC0~3
	GPIOC->CRL|=0X00003333; 
	GPIOC->CRH&=0XFF0FFFFF;//PC13
	GPIOC->CRH|=0X00300000;//PC13推挽输出 
//	GPIOC->ODR|=0X200f;    //PC0~3 13 全部上拉	   
	PCout(0) = PCout(2) = PCout(3) = PCout(13) = 0;
	hw_pwm_init(1000-1,72-1);	// 72MHz 72分频，时基1M，周期1K
}

void hw_pwm_init(u16 arr,u16 psc)
{

// pa0
	GPIOA->CRL &= 0xFFFFFFF0;
	GPIOA->CRL |= 0xA;	// 速度2m，复用推挽
// pa1
	GPIOA->CRL &= 0xFFFFFF0F;
	GPIOA->CRL |= 0xA0;	// 速度2m，复用推挽

// pa8:led port for test
//	GPIOA->CRH&=0XFFFFFFF0;
//	GPIOA->CRH|=0X00000004;//PA8浮空输入



// enable the timer2 power
	RCC->APB1ENR |= 1;
// timer2
	
	// reset the control register
	TIM2->CR1 = 0;
	TIM2->CR2 = 0;
	TIM2->SMCR = 0;
	TIM2->DIER = 0;
	TIM2->SR = 0;
	TIM2->EGR = 0;
	TIM2->CCMR2 = 0;
	TIM2->DCR = 0;	// DMA control register
	TIM2->DMAR = 0;

	// set the control register
	TIM2->CCMR1 = ( 6<<12 | 6<<4 ); // set ch1 & ch2 mode.
  	//TIM2->CCER = 1;	//enable ch1
	TIM2->CCER = ( 1 | 1<<4 );	// enable ch1 & ch2 output.
	TIM2->CNT = 0;
	TIM2->PSC=psc;
	TIM2->ARR=arr;	// pwm freq should <10k for L298N
	TIM2->CCR1 = 0;
	TIM2->CCR2 = 0;

// start the timer
	TIM2->CR1 = 1;

//    TIM2->CCMR1 |= 7<<12;
//    TIM2->CCER |= 1<<4;
//    TIM2->CR1 |= 0x8000;
//    TIM2->CR1 |= 0x01;

}

// motor: 0~1000
void pwm_control(s16 motor1, s16 motor2)
{
#if 1
	if(motor1>0) motor1 += 15;
	else if(motor1<0)motor1 -=15;

	if(motor2>0) motor2 += 15;
	else if(motor2<0) motor2 -=15;
#endif

	if(motor1>1000) motor1 = 1000;
	else if(motor1<-1000) motor1 = -1000;
	if(motor2>1000) motor2 = 1000;
	else if(motor2<-1000) motor2 = -1000;

	if(motor1==0)
	{
		PCout(3) = 0;
		PCout(2) = 0;
	}
	else if(motor1>0)
	{
		PCout(3) = 1;
		PCout(2) = 0;
		TIM2->CCR1 = motor1;
	}
	else
	{
		PCout(3) = 0;
		PCout(2) = 1;
		TIM2->CCR1 = -motor1;
	}

	if(motor2==0)
	{
		PCout(0)  = 0;
		PCout(13) = 0;
	}
	else if(motor2>0)
	{
		PCout(0)  = 1;
		PCout(13) = 0;
		TIM2->CCR2 = motor2;
	}
	else
	{
		PCout(0)  = 0;
		PCout(13) = 1;
		TIM2->CCR2 = -motor2;
	}
}
