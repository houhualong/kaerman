/*
* File:		support_common.h
 * Purpose:		Various project configurations.
 *
 * Notes:
 */

#ifndef _SUPPORT_COMMON_H_
#define _SUPPORT_COMMON_H_


#define MEMORY_INIT \
	/* Initialize RAMBAR: locate SRAM and validate it */ \
	move.l	%#__RAMBAR + 0x21,d0; \
	movec	d0,RAMBAR;



#endif /* _SUPPORT_COMMON_H_ */

