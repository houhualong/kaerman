/** ###################################################################
**     Filename  : Display.H
**     Project   : CF2_0
**     Processor : MCF52255CAF80
**     Compiler  : CodeWarrior MCF C Compiler
**     Date/Time : 2011-12-16, 6:05
**     Contents  :
**         User source code
**
** ###################################################################*/

#ifndef __Display_H
#define __Display_H

/* MODULE Display */
#include "Cpu.h"
#include "Events.h"
#include "ADCon.h"
#include "MotorR.h"
#include "MotorL.h"
#include "UART.h"
#include "CtrlTimer.h"

/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

extern void Dis(void);
extern void Dis_Init(void);
extern void SetCharBuf(char *p);
extern void Set1FloatBuf(float f);
extern void Set1IntBuf(int16 i);
extern void Set2IntBuf(int16 i, int16 j);
extern void Set2uIntBuf(uint16 i, uint16 j);
extern uint8 dis_buf[7];
/* END Display */

#endif

