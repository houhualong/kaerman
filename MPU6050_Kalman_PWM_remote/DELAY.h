/*--------------------------------------------------------------------------
DELAY.H

延时函数 头文件
--------------------------------------------------------------------------*/

#ifndef __DELAY_H__
#define __DELAY_H__

#define uchar unsigned char
#define uint  unsigned int


//------函数（声明）--------------------------------
void Delay400Ms(void);								//延时400mS函数（子程序）
void delay1ms(void);								//延时1mS函数（子程序）
void delaynms(uchar n);							    //延时n mS函数（子程序）



//***********************************************************************************/
 //延时400ms

void Delay400Ms(void)
{
 	uchar TempCycA = 30;		//1周期MPU用30,  12周期MPU用5
 	uint TempCycB;
 	while(TempCycA--)
		{
		TempCycB=7269;
		while(TempCycB--);
		};
}



//***********************************************************************************/
 //延时1ms

 void delay1ms()
 {
   uchar i,j;	
   for(i=0;i<10;i++)
   for(j=0;j<33;j++)
	   ;		 
  }



//***********************************************************************************/
 //延时n*ms

 void delaynms(uchar n)
 {
   uchar i;
   for(i=0;i<n;i++) delay1ms();

 }

#endif
