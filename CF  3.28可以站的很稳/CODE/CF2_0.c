/** ###################################################################
**     Filename  : CF2_0.C
**     Project   : CF2_0
**     Processor : MCF52255CAF80
**     Version   : Driver 01.00
**     Compiler  : CodeWarrior MCF C Compiler
**     Date/Time : 2011-12-15, 1:20
**     Abstract  :
**         Main module.
**         This module contains user's application code.
**     Settings  :
**     Contents  :
**         No public methods
**
** ###################################################################*/
/* MODULE CF2_0 */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
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
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "DataProcess.h"

extern int16 Kp,Kd;

void main(void)
{
  /* Write your local variable definition here */
  
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
   PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/
  
  /* Write your code here */
  /* For example: for(;;) { } */
  CarInit();
  
  CtrlTimer_Enable();
  
  for(;;)
  {
  	if(!KEY0_GetVal())
  	{
  		Kp++;
  		LED0_NegVal();
  		Cpu_Delay100US(2000);
  		LED0_NegVal();
  	}
  	if(!KEY1_GetVal())
  	{
  		Kp--;
  		LED0_NegVal();
  		Cpu_Delay100US(2000);
  		LED0_NegVal();
  	}
  	if(!KEY2_GetVal())
  	{
  		Kd++;
  		LED0_NegVal();
  		Cpu_Delay100US(2000);
  		LED0_NegVal();
  	}
  	if(!KEY3_GetVal())
  	{
  		Kd--;
  		LED0_NegVal();
  		Cpu_Delay100US(2000);
  		LED0_NegVal();
  	}
  }
  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END CF2_0 */
/*
** ###################################################################
**
**     This file was created by Processor Expert 1.05 [04.27]
**     for the Freescale MCF series of microcontrollers.
**
** ###################################################################
*/
