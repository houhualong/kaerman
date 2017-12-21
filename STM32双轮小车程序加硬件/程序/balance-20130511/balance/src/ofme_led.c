#include "ofme_led.h"


void led_init(void)
{

	unsigned int temp;

	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��	   	 
	temp = GPIOA->CRH;
	temp &=0XFFFFFFF0; 
	temp |= 0X00000003;
	LED0_OFF();
	GPIOA->CRH = temp;//PA8 �������50MHz   	 


	RCC->APB2ENR|=1<<5;    //ʹ��PORTDʱ��	
	temp = GPIOD->CRL;
	temp &= 0XFFFFF0FF;
	temp |= 0X00000300;
	LED1_OFF();
	GPIOD->CRL = temp;//PD2�������50MHz
}
