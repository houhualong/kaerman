/** ###################################################################
**     Filename  : Events.C
**     Project   : CF2_0
**     Processor : MCF52255CAF80
**     Beantype  : Events
**     Compiler  : CodeWarrior MCF C Compiler
**     Date/Time : 2011-12-15, 1:20
**     Abstract  :
**         This is user's event module.
**         Put your event handler code here.
**     Settings  :
**     Contents  :
**         Cpu_OnCoreWatchdogINT - void Cpu_OnCoreWatchdogINT(void);
**
** ###################################################################*/
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "DataProcess.h"
#include "Speed_PID.h"

/*
** ===================================================================
**     Event       :  Cpu_OnCoreWatchdogINT (module Events)
**
**     From bean   :  Cpu [MCF52255_100_LQFP]
**     Description :
**         This event is called when the OnCoreWatchdog interrupt had
**         occurred. This event is automatically enabled when the <Mode>
**         is set to 'Interrupt'.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Cpu_OnCoreWatchdogINT(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  CtrlTimer_OnInterrupt (module Events)
**
**     From bean   :  CtrlTimer [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the bean is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
int8 time_1ms = 0;
int8 time_5ms = 0;


void CtrlTimer_OnInterrupt(void)
{
  /* Write your code here ... */
  uint16 tmp1 = 0, tmp2 = 0;
  time_1ms++;
  if(time_1ms == 1) // 第一个1ms
  {
  	ADCon_Measure(FALSE);//开始采样
  	LeftCnt_GetNumEvents(&tmp1);//读取编码器
  	RightCnt_GetNumEvents(&tmp2);
  	if(LeftDir_GetVal())//L 正转
	{
		NowSpeed_L += (int16)tmp1;
	}
	else//反转
	{
		NowSpeed_L -= (int16)tmp1;
	}
	if(RightDir_GetVal())//R 正转
	{
		NowSpeed_R += (int16)tmp2;
	}
	else//反转
	{
		NowSpeed_R -= (int16)tmp2;
	}
  	LeftCnt_Reset();
  	RightCnt_Reset();
  }
  else if(time_1ms == 2)// 第二个1ms
  {
  	 SensorDataProcess();
  }
  else if(time_1ms == 3)//第三个1ms
  {
  	CarAngleCtrl();
  }
  else if(time_1ms == 4)//第四个1ms
  {
  	time_5ms++;
  	if(time_5ms >= 20) //周期100ms
  	{
  		time_5ms = 0;
  		Speed_PI();
  		NowSpeed_L = 0;
  		NowSpeed_R = 0;
  	//	LED0_NegVal();
  	}
  	
  	Speed_PI_OUT();                    //速度输出控制，周期5ms
  }
  else if(time_1ms == 5)
  {
  	time_1ms = 0;
  }
}

/*
** ===================================================================
**     Event       :  DisTimer_OnInterrupt (module Events)
**
**     From bean   :  DisTimer [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the bean is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/


/*
** ===================================================================
**     Event       :  TI1_OnInterrupt (module Events)
**
**     From bean   :  TI1 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the bean is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void TI1_OnInterrupt(void)
{
  /* Write your code here ... */
  LED0_NegVal();
}

/* END Events */

/*
** ###################################################################
**
**     This file was created by Processor Expert 1.05 [04.27]
**     for the Freescale MCF series of microcontrollers.
**
** ###################################################################
*/
