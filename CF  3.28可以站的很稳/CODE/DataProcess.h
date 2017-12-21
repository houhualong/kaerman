/** ###################################################################
**     Filename  : DataProcess.H
**     Project   : CF2_0
**     Processor : MCF52255CAF80
**     Compiler  : CodeWarrior MCF C Compiler
**     Date/Time : 2011-12-15, 6:56
**     Contents  :
**         User source code
**
** ###################################################################*/

#ifndef __DataProcess_H
#define __DataProcess_H

/* MODULE DataProcess */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "ADCon.h"
#include "MotorL.h"
#include "MotorR.h"
#include "UART.h"
#include "CtrlTimer.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "LeftDir.h"
#include "RightDIr.h"

#define GYRO 0x00
#define ACC  0x01


extern int32 Real_Left_Speed;
extern int32 Real_Right_Speed;

extern void CarInit(void);
extern void SensorDataProcess(void);
extern void CarAngleCtrl(void);
/* END DataProcess */

#endif

