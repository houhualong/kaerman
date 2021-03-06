/** ###################################################################
**     THIS BEAN MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : UART.H
**     Project   : CF2_0
**     Processor : MCF52255CAF80
**     Beantype  : AsynchroSerial
**     Version   : Bean 02.465, Driver 01.02, CPU db: 3.00.000
**     Compiler  : CodeWarrior MCF C Compiler
**     Date/Time : 2012-3-23, 16:13
**     Abstract  :
**         This bean "AsynchroSerial" implements an asynchronous serial
**         communication. The bean supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The bean requires one on-chip asynchronous serial channel.
**     Settings  :
**         Serial channel              : UART0
**
**         Protocol
**             Init baud rate          : 9600baud
**             Width                   : 8 bits
**             Stop bits               : 1.000
**             Parity                  : none
**             Breaks                  : Disabled
**             Input buffer size       : 0
**             Output buffer size      : 0
**
**         Registers
**             Input buffer            : URB0      [0x020C]
**             Output buffer           : UTB0      [0x020C]
**             Control register        : UMR10     [0x0200]
**             Mode register           : UMR20     [0x0200]
**             Baud setting reg.       : UCSR0     [0x0204]
**
**
**
**         Used pins:
**         ----------------------------------------------------------
**           Function | On package           |    Name
**         ----------------------------------------------------------
**            Input   |     11               |  URXD0_PUA1
**            Output  |     10               |  UTXD0_PUA0
**         ----------------------------------------------------------
**
**
**
**     Contents  :
**         Enable   - byte UART_Enable(void);
**         Disable  - byte UART_Disable(void);
**         RecvChar - byte UART_RecvChar(UART_TComData *Chr);
**         SendChar - byte UART_SendChar(UART_TComData Chr);
**
**     Copyright : 1997 - 2009 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef __UART
#define __UART

/* MODULE UART. */

#include "Cpu.h"



#ifndef __BWUserType_UART_TError
#define __BWUserType_UART_TError
  typedef union {
    byte err;
    struct {
      bool OverRun  : 1;               /* Overrun error flag */
      bool Framing  : 1;               /* Framing error flag */
      bool Parity   : 1;               /* Parity error flag */
      bool RxBufOvf : 1;               /* Rx buffer full error flag */
      bool Noise    : 1;               /* Noise error flag */
      bool Break    : 1;               /* Break detect */
      bool LINSync  : 1;               /* LIN synchronization error */
      bool BitError  : 1;              /* Bit error flag - mismatch to the expected value happened. */
    } errName;
  } UART_TError;                       /* Error flags. For languages which don't support bit access is byte access only to error flags possible. */
#endif

#ifndef __BWUserType_UART_TComData
#define __BWUserType_UART_TComData
  typedef byte UART_TComData;          /* User type for communication. Size of this type depends on the communication data witdh */
#endif



byte UART_Enable(void);
/*
** ===================================================================
**     Method      :  UART_Enable (bean AsynchroSerial)
**
**     Description :
**         Enables the bean - it starts the send and receive
**         functions. Events may be generated
**         ("DisableEvent"/"EnableEvent").
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/

byte UART_Disable(void);
/*
** ===================================================================
**     Method      :  UART_Disable (bean AsynchroSerial)
**
**     Description :
**         Disables the bean - it stops the send and receive
**         functions. No events will be generated.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/

byte UART_RecvChar(UART_TComData *Chr);
/*
** ===================================================================
**     Method      :  UART_RecvChar (bean AsynchroSerial)
**
**     Description :
**         If any data is received, this method returns one
**         character, otherwise it returns an error code (it does
**         not wait for data). This method is enabled only if the
**         receiver property is enabled.
**         [Note:] Because the preferred method to handle error and
**         break exception in the interrupt mode is to use events
**         <OnError> and <OnBreak> the return value ERR_RXEMPTY has
**         higher priority than other error codes. As a consequence
**         the information about an exception in interrupt mode is
**         returned only if there is a valid character ready to be
**         read.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Chr             - Pointer to a received character
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_RXEMPTY - No data in receiver
**                           ERR_BREAK - Break character is detected
**                           (only when the <Interrupt service>
**                           property is disabled and the <Break
**                           signal> property is enabled)
**                           ERR_COMMON - common error occurred (the
**                           <GetError> method can be used for error
**                           specification)
** ===================================================================
*/

byte UART_SendChar(UART_TComData Chr);
/*
** ===================================================================
**     Method      :  UART_SendChar (bean AsynchroSerial)
**
**     Description :
**         Sends one character to the channel. If the bean is
**         temporarily disabled (Disable method) SendChar method
**         only stores data into an output buffer. In case of a zero
**         output buffer size, only one character can be stored.
**         Enabling the bean (Enable method) starts the transmission
**         of the stored data. This method is available only if the
**         transmitter property is enabled.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Chr             - Character to send
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_TXFULL - Transmitter is full
** ===================================================================
*/

void UART_Init(void);
/*
** ===================================================================
**     Method      :  UART_Init (bean AsynchroSerial)
**
**     Description :
**         Initializes the associated peripheral(s) and the bean internal 
**         variables. The method is called automatically as a part of the 
**         application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/



/* END UART. */

/*
** ###################################################################
**
**     This file was created by Processor Expert 1.05 [04.27]
**     for the Freescale MCF series of microcontrollers.
**
** ###################################################################
*/

#endif /* ifndef __UART */
