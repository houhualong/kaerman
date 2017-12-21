/** ###################################################################
**     Filename  : Key.H
**     Project   : CF2_0
**     Processor : MCF52255CAF80
**     Compiler  : CodeWarrior MCF C Compiler
**     Date/Time : 2011-12-16, 9:23
**     Contents  :
**         User source code
**
** ###################################################################*/

#ifndef __Key_H
#define __Key_H

/* MODULE Key */

#define KEY_SETUP  0x01
#define KEY_RUN    0x02
#define KEY_LEFT   0x03
#define KEY_RIGHT  0x04
#define KEY_UP     0x05
#define KEY_DOWN 0x06
#define KEY_NONE 0x00

extern uint8 Key_Scan(void);

/* END Key */

#endif

