/*============================================================================*/
/*
SimpleRTOS - Very simple RTOS for Microcontrollers - PIC24 port
v2.00 (2014-01-21)
isaacbavaresco@yahoo.com.br
*/
/*============================================================================*/
/*
 Copyright (c) 2007-2014, Isaac Marino Bavaresco
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Neither the name of the author nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*============================================================================*/
#if         !defined __PORTPIC24DSPICINTERNALS_H__
#define __PORTPIC24DSPICINTERNALS_H__
/*============================================================================*/
#include "SimpleRTOS.h"
/*============================================================================*/
typedef unsigned short	intsave_t;
/*============================================================================*/
#define MAX_SYSCALL_INTERRUPT_PRIORITY	0x03
#define IPL_SHIFT						(10UL)
#define ALL_IPL_BITS					(0x3fUL<<IPL_SHIFT)
/*============================================================================*/
extern inline intsave_t __attribute((always_inline)) SaveAndDisableInterrupts( void )
	{
	register intsave_t	Status;

	Status	= SR;
	SR		= ( SR & 0xff1f ) | 0x00e0;

	return Status;
	}
/*============================================================================*/
extern inline void __attribute((always_inline)) RestoreInterrupts( intsave_t Status )
	{
	SR		= ( SR & 0xff1f ) | ( Status & 0x00e0 );
	}
/*============================================================================*/
void                        HigherPriorityAwakened	( void );
void __attribute__((naked)) ForceYield              ( void );
void __attribute__((naked)) SaveContext             ( void );
void __attribute__((naked)) RestoreContext          ( void );
/*============================================================================*/
#endif	/*	!defined __PORTPIC24DSPICINTERNALS_H__ */
/*============================================================================*/
