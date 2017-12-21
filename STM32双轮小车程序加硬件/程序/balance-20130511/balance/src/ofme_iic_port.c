#include "ofme_iic.h"

void hw_iic_init(void)
{					     
#if (IIC_PORT==1)

 	RCC->APB2ENR|=1<<4;//��ʹ������IO PORTCʱ�� 							 
	GPIOC->CRH&=0XFFF00FFF;
	GPIOC->CRH |= 0x00077000;
	GPIOC->ODR|=3<<11;     //PC11,12 �����
#elif (IIC_PORT==2)
//PC4 -NRF_CS  JF24_CS	---->GND
//PA7 -SPI1_MOSI		---->SDA  	
//PC5 -NRF_IRQ JF24_BKT	---->SCL
	RCC->APB2ENR |= ((1<<4)+(1<<2))	;	// ʹ��PA��PCʱ��
	GPIOC->CRL &= 0xFFF0FFFF;
	GPIOC->CRL |= 1<<16;//�������
	GPIOC->BRR = 1<<4;// PC4���0

	GPIOA->CRL &= 0x0FFFFFFF;
	GPIOA->CRL |= 0x70000000;//��©����
	GPIOA->ODR |= 1<<7;

	GPIOC->CRL &= 0xFF0FFFFF;
	GPIOC->CRL |= 0x00700000;//��©����
	GPIOC->ODR |= 1<<5;
#endif
}
