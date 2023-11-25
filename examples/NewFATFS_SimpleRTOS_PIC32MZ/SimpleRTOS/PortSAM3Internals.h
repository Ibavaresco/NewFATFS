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
#if			!defined __PORTSAM3INTERNALS_H__
#define __PORTSAM3INTERNALS_H__
/*============================================================================*/
#include <asf.h>
#include "SimpleRTOS.h"
/*============================================================================*/
typedef unsigned long	intsave_t;
/*============================================================================*/
#define KERNEL_INTERRUPT_PRIORITY		(15)
#define MAX_SYSCALL_INTERRUPT_PRIORITY	(8ul)

//extern void RestoreInterrupts( intsave_t Status );
//extern intsave_t SaveAndDisableInterrupts( void );

/*============================================================================*/

#define	USE_PSP

/*============================================================================*/

#if			defined USE_PSP

#define SAVE_CONTEXT()															\
	__asm volatile(																\
		"mrs	r12,basepri					\n"									\
																				\
		"mov	r0,%0						\n"									\
		"msr	basepri,r0					\n"									\
																				\
		"mrs	r1,psp						\n"									\
		"isb								\n"		\
		"stmdb	r1!,{r4-r12}				\n"	/* Save the remaining registers. */	\
																				\
		"ldr	r0,=CurrentTask				\n"	/*	r0	= &CurrentTask		*/	\
		"ldr	r0,[r0]						\n"	/*	r0	 = CurrentTask		*/	\
		"str	r1, [r0]					\n"	/*	CurrentTask->sp = sp		*/	\
																				\
		"stmdb	r13!,{r14}					\n"									\
																				\
		:: "i"(( MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - __NVIC_PRIO_BITS )) & 0xff ) : "r0"	\
		);

#define RESTORE_CONTEXT()														\
	__asm volatile(																\
		"ldmia	r13!,{r14}					\n"									\
																				\
		"ldr	r0,=CurrentTask				\n"	/*	r0	= &CurrentTask		*/	\
		"ldr	r0,[r0]						\n"	/*	r0	 = CurrentTask		*/	\
		"ldr	r1,[r0]						\n"	/*	sp	= CurrentTask->sp*/		\
																				\
		"ldmia	r1!,{r4-r12}				\n"  /* Load the remaining registers. */	\
		"msr	psp,r1						\n"									\
		"isb								\n"									\
																				\
		"msr	basepri,r12					\n"									\
																				\
		"bx		r14							\n"									\
		);

#else	/*	defined USE_PSP */

#define SAVE_CONTEXT()															\
	__asm volatile(																\
		"mrs	r12,basepri					\n"									\
																				\
		"mov	r0,%0						\n"									\
		"msr	basepri,r0					\n"									\
																				\
		"stmdb	r13!, {r4-r12}				\n"	/* Save the remaining registers. */	\
																				\
		"ldr	r0,=CurrentTask				\n"	/*	r0	= &CurrentTask		*/	\
		"ldr	r0, [r0]					\n"	/*	r1	 = CurrentTask		*/	\
		"str	r13, [r0]					\n"	/*	CurrentTask->sp = sp		*/	\
																				\
		"stmdb	r13!,{r14}					\n"									\
																				\
		:: "i"(( MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - __NVIC_PRIO_BITS )) & 0xff ) : "r0"	\
		);

#define RESTORE_CONTEXT()														\
	__asm volatile(																\
		"ldmia	r13!,{r14}					\n"									\
																				\
		"ldr	r0,=CurrentTask				\n"	/*	r0	= &CurrentTask	*/		\
		"ldr	r0, [r0]					\n"	/*	r0	 = CurrentTask	*/		\
		"ldr	r13,[r0]					\n"	/*	sp	= CurrentTask->sp*/		\
																				\
		"ldmia	r13!, {r4-r12}				\n"  /* Load the remaining registers. */	\
																				\
		"msr	basepri,r12					\n"									\
																				\
		"bx		r14							\n"									\
																				\
		::: "r0"																\
		);

#endif	/*	defined USE_PSP */

/*============================================================================*/

static inline intsave_t __attribute((always_inline)) SaveAndDisableInterrupts( void )
	{
	register intsave_t	Status;

	asm volatile(
		"mrs	r0,basepri	\n"
		"mov	%0,r0		\n"
		"mov	r0,%1		\n"
		"msr	basepri,r0	\n"
		: "=r"( Status ) : "i"(( MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - __NVIC_PRIO_BITS )) & 0xff ) : "r0"
		);

	return Status;
	}
/*============================================================================*/
static inline void __attribute((always_inline)) RestoreInterrupts( intsave_t Status )
	{
	asm volatile(
		"mov	r0,%0		\n"
		"msr	basepri,r0	\n"
		:: "r"( Status ) : "r0"
		);
	}
/*============================================================================*/
static inline void __attribute((always_inline)) ForceYield( void )
	{
	asm volatile( "svc 0" );
	}
/*============================================================================*/
#endif	/*	!defined __PORTSAM3INTERNALS_H__ */
/*============================================================================*/
