/*--------------------------------------------------------------------------
DELAY.H

��ʱ���� ͷ�ļ�
--------------------------------------------------------------------------*/

#ifndef __DELAY_H__
#define __DELAY_H__

#define uchar unsigned char
#define uint  unsigned int


//------������������--------------------------------
void Delay400Ms(void);								//��ʱ400mS�������ӳ���
void delay1ms(void);								//��ʱ1mS�������ӳ���
void delaynms(uchar n);							    //��ʱn mS�������ӳ���



//***********************************************************************************/
 //��ʱ400ms

void Delay400Ms(void)
{
 	uchar TempCycA = 30;		//1����MPU��30,  12����MPU��5
 	uint TempCycB;
 	while(TempCycA--)
		{
		TempCycB=7269;
		while(TempCycB--);
		};
}



//***********************************************************************************/
 //��ʱ1ms

 void delay1ms()
 {
   uchar i,j;	
   for(i=0;i<10;i++)
   for(j=0;j<33;j++)
	   ;		 
  }



//***********************************************************************************/
 //��ʱn*ms

 void delaynms(uchar n)
 {
   uchar i;
   for(i=0;i<n;i++) delay1ms();

 }

#endif
