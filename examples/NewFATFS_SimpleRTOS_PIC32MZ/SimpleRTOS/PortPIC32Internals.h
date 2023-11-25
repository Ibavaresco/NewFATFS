/*============================================================================*/
/*
SimpleRTOS - Very simple RTOS for Microcontrollers - PIC32 port
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
#if			!defined __PORTPIC32INTERNALS_H__
#define __PORTPIC32INTERNALS_H__
/*============================================================================*/
#define KERNEL_INTERRUPT_PRIORITY		0x01
#define MAX_SYSCALL_INTERRUPT_PRIORITY	0x04
#define IPL_SHIFT						( 10UL )
#define ALL_IPL_BITS					( 0x3fUL << IPL_SHIFT )
/*============================================================================*/
#define	USE_CORE_TIMER                  0
/*============================================================================*/
#if         !defined __ASSEMBLER__
/*============================================================================*/
#include <p32xxxx.h>
#include "SimpleRTOS.h"
/*============================================================================*/
typedef unsigned long	intsave_t;
/*============================================================================*/
static inline intsave_t __attribute((always_inline)) SaveAndDisableInterrupts( void )
	{
	register intsave_t	Status;

	Status	 = _CP0_GET_STATUS();
   	_CP0_SET_STATUS(( Status & ~ALL_IPL_BITS ) | ( MAX_SYSCALL_INTERRUPT_PRIORITY << IPL_SHIFT ));

	return Status;
	}
/*============================================================================*/
static inline void __attribute((always_inline)) RestoreInterrupts( intsave_t Status )
	{
	_CP0_SET_STATUS(( _CP0_GET_STATUS() & ~ALL_IPL_BITS ) | ( Status & ALL_IPL_BITS ));
	}
/*============================================================================*/
void HigherPriorityAwakened	( void );
void ForceYield				( void );
void SaveFP                 ( fpucontext_t * );
void RestoreFP              ( fpucontext_t * );
void RestoreContext         ( void );
/*============================================================================*/
#define CallInterruptHandler(name)          \
	asm volatile(                           \
		".extern	SaveContext		\r\n"   \
		".extern	RestoreContext	\r\n"   \
		".extern "	#name          "\r\n"   \
                                            \
		"addiu		$sp,$sp,-8		\r\n"   \
		"sw			$ra,4($sp)		\r\n"   \
                                            \
		"jal		SaveContext		\r\n"   \
		"nop						\r\n"   \
                                            \
        "jal "      #name         "\r\n"   \
		"nop						\r\n"   \
                                            \
		"j			RestoreContext	\r\n"   \
		"nop						\r\n"   \
		);
/*============================================================================*/
#endif  /*  !defined __ASSEMBLER__ */
/*============================================================================*/
#endif	/*	!defined __PORTPIC32INTERNALS_H__ */
/*============================================================================*/
