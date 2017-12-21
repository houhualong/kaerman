#include "ofme_iic.h"

void hw_iic_init(void)
{					     
#if (IIC_PORT==1)

 	RCC->APB2ENR|=1<<4;//先使能外设IO PORTC时钟 							 
	GPIOC->CRH&=0XFFF00FFF;
	GPIOC->CRH |= 0x00077000;
	GPIOC->ODR|=3<<11;     //PC11,12 输出高
#elif (IIC_PORT==2)
//PC4 -NRF_CS  JF24_CS	---->GND
//PA7 -SPI1_MOSI		---->SDA  	
//PC5 -NRF_IRQ JF24_BKT	---->SCL
	RCC->APB2ENR |= ((1<<4)+(1<<2))	;	// 使能PA、PC时钟
	GPIOC->CRL &= 0xFFF0FFFF;
	GPIOC->CRL |= 1<<16;//推挽输出
	GPIOC->BRR = 1<<4;// PC4输出0

	GPIOA->CRL &= 0x0FFFFFFF;
	GPIOA->CRL |= 0x70000000;//开漏输入
	GPIOA->ODR |= 1<<7;

	GPIOC->CRL &= 0xFF0FFFFF;
	GPIOC->CRL |= 0x00700000;//开漏输入
	GPIOC->ODR |= 1<<5;
#endif
}
