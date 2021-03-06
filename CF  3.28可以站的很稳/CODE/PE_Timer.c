/** ###################################################################
**     THIS BEAN MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : PE_Timer.C
**     Project   : CF2_0
**     Processor : MCF52255CAF80
**     Beantype  : PE_Timer
**     Version   : Driver 01.01
**     Compiler  : CodeWarrior MCF C Compiler
**     Date/Time : 2011-12-15, 8:34
**     Abstract  :
**         This module "PE_Timer" implements internal methods and definitions
**         used by beans working with timers.
**     Settings  :
**     Contents  :
**         No public methods
**
**     Copyright : 1997 - 2009 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/


/* MODULE PE_Timer. */

#include "PE_Timer.h"

typedef union {
  dlong value;
  struct {
    dword part0;
    dword part1;
  } DW;
  struct {
    word part0;
    word part1;
    word part2;
    word part3;
  } W;
  struct {
    byte part0;
    byte part1;
    byte part2;
    byte part3;
    byte part4;
    byte part5;
    byte part6;
    byte part7;
  } B;
} PE_Timer_UINT64;

/*
** ===================================================================
**     Method      :  PE_Timer_LngHi2 (bean PE_Timer)
**
**     Description :
**         64bits number / 2^24 -> 16bits number. The method is called 
**         automatically as a part of several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
bool PE_Timer_LngHi2(dlong In, word *Out) 
{
  PE_Timer_UINT64 *Input = (PE_Timer_UINT64*)&In;

  if (Input->DW.part0 == 0) {
    if ((Input->W.part3 & 0x8000) != 0) {
      if (Input->W.part2 < 0xFFFF) {
        *Out = (word)(Input->W.part2 + 1);
        return FALSE;
      }  
    }
    else {
      *Out = Input->W.part2;
      return FALSE;
    }  
  }
  *Out = Input->W.part2;
  return TRUE;
}

/* END PE_Timer. */

/*
** ###################################################################
**
**     This file was created by Processor Expert 1.05 [04.27]
**     for the Freescale MCF series of microcontrollers.
**
** ###################################################################
*/
