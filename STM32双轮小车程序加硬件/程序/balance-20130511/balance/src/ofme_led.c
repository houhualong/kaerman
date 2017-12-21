#include "ofme_led.h"


void led_init(void)
{

	unsigned int temp;

	RCC->APB2ENR|=1<<2;    //使能PORTA时钟	   	 
	temp = GPIOA->CRH;
	temp &=0XFFFFFFF0; 
	temp |= 0X00000003;
	LED0_OFF();
	GPIOA->CRH = temp;//PA8 推挽输出50MHz   	 


	RCC->APB2ENR|=1<<5;    //使能PORTD时钟	
	temp = GPIOD->CRL;
	temp &= 0XFFFFF0FF;
	temp |= 0X00000300;
	LED1_OFF();
	GPIOD->CRL = temp;//PD2推挽输出50MHz
}
