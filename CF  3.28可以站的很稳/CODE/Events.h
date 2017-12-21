/** ###################################################################
**     Filename  : Events.H
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

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "ADCon.h"
#include "MotorR.h"
#include "MotorL.h"
#include "UART.h"
#include "CtrlTimer.h"
#include "LED0.h"
#include "LeftCnt.h"
#include "RightCnt.h"
#include "LeftDir.h"
#include "RightDir.h"
#include "KEY0.h"
#include "KEY1.h"
#include "KEY2.h"
#include "KEY3.h"


extern int8 time_5ms;
extern int8 time_1ms;
void Cpu_OnCoreWatchdogINT(void);
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


void CtrlTimer_OnInterrupt(void);
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

void DisTimer_OnInterrupt(void);
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

void TI1_OnInterrupt(void);
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

/* END Events */
#endif /* __Events_H*/

/*
** ###################################################################
**
**     This file was created by Processor Expert 1.05 [04.27]
**     for the Freescale MCF series of microcontrollers.
**
** ###################################################################
*/
