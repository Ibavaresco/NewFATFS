/*============================================================================*/
/*
SimpleRTOS - Very simple RTOS for Microcontrollers - ARM Cortex-M3 ATSAM3 port
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
#include <asf.h>
#include <string.h>
#include "PortSAM3.h"
#include "SimpleRTOSInternals.h"
/*============================================================================*/
//extern void __attribute__((weak)) TickHook( void );
/*============================================================================*/
static void __attribute__((noinline)) Switch( void )
	{
	SystemTick++;

	CheckDelayList();

	/* The current task ran for at least one full time slice... */
	if( AlreadySwitched == 0 && CurrentTask->Priority == HighestReadyPriority )
		/* ... it doesn't deserve an additional time slice. */
		ReadyTasks[HighestReadyPriority]
			= ReadyTasks[HighestReadyPriority]->Next;

	CurrentTask			= ReadyTasks[HighestReadyPriority];

	/*
	The upcoming task will run from the very beginning of its time slice,
	at the end of this slice it will be switched off.
	*/
	AlreadySwitched		= 0;

    if( TickHook != NULL )
        TickHook();
	}
/*============================================================================*/
/* Constants required to manipulate the NVIC. */
#define NVIC_SYST_CSR			(*(volatile unsigned long *)0xe000e010)
#define NVIC_SYST_CSR_ENABLE	0x00000001
#define NVIC_SYST_CSR_TICKINT	0x00000002
#define NVIC_SYST_CSR_CLKSOURCE	0x00000004

#define NVIC_SYST_RVR			( *(volatile unsigned long*)0xe000e014 )
#define NVIC_SYST_CVR			( *(volatile unsigned long*)0xe000e018 )

#define NVIC_SHPR2				( *(volatile unsigned long*)0xe000ed1c )
#define NVIC_SHPR2_SVCALL_PRI	(( (unsigned long)((( MAX_SYSCALL_INTERRUPT_PRIORITY - 1 ) << ( 8 - __NVIC_PRIO_BITS )) & 0xff )) << 24 )

#define NVIC_SHPR3				( *(volatile unsigned long*)0xe000ed20 )
#define NVIC_SHPR3_SYSTICK_PRI	(( (unsigned long)(( KERNEL_INTERRUPT_PRIORITY << ( 8 - __NVIC_PRIO_BITS )) & 0xff )) << 24 )

/*============================================================================*/

void __attribute__ ((naked)) SysTick_Handler( void )
	{
	SAVE_CONTEXT();

	Switch();

	RESTORE_CONTEXT();
	}

/*============================================================================*/
void __attribute__ ((naked)) SVC_Handler( void )
	{
	SAVE_CONTEXT();

	CurrentTask	= ReadyTasks[HighestReadyPriority];

	RESTORE_CONTEXT();
	}

/*============================================================================*/

void ContextInit( context_t *Context, unsigned char *Stack, unsigned long StackSize, void *Parameter, void (*TaskFunc)( void* ), unsigned int Priority )
	{
	unsigned long	*StackTop;

	memset( Context, 0x00, sizeof( context_t ));

	StackTop			= (unsigned long *)(( (unsigned long)Stack + StackSize ) & ~7 );

	/* Registers saved automatically */
	*--StackTop	= 0x01000000ul;								/* PSR */
	*--StackTop	= (unsigned long)TaskFunc | 0x00000001ul;	/* r15 (PC) */
	*--StackTop	= (unsigned long)TaskFinish;				/* r14 (LR)) */
	*--StackTop	= 0;										/* r12 */
	*--StackTop	= 0;										/* r3 */
	*--StackTop	= 0;										/* r2 */
	*--StackTop	= 0;										/* r1 */
	*--StackTop	= (unsigned long)Parameter;					/* r0 */
	*--StackTop	= 0;										/* r11*/
	*--StackTop	= 0;										/* r10 */
	*--StackTop	= 0;										/* r9 */
	*--StackTop	= 0;										/* r8 */
	*--StackTop	= 0;										/* r7 */
	*--StackTop	= 0;										/* r6 */
	*--StackTop	= 0;										/* r5 */
	*--StackTop	= 0;										/* r4 */

	Context->sp						= (unsigned long)StackTop;
	Context->basepri				= 0;
	Context->Errno					= 0;

	Context->Priority				= Priority;
	Context->TDelay					= 0;
	Context->PreviousDelayedTask	= NULL;
	Context->NextDelayedTask		= NULL;
	Context->DelayList				= NULL;

#ifdef		USE_SLICE_LENGTH
	Context->SliceLength		= 0;
#endif	/*	USE_SLICE_LENGTH */

	Context->Previous				= NULL;
	Context->Next					= NULL;
	Context->List					= NULL;
	}
/*============================================================================*/
void InitRTOS( context_t *Contexts, unsigned int Number )
	{
	unsigned int i;

	/* No contexts to be initialized... The RTOS is useless.*/
	if( Contexts == NULL || Number == 0 )
		{
		FreeContexts	= NULL;
		return;
		}

	CurrentTask		= Contexts;

	for( i = 0; i < MaxPriorities; i++ )
		ReadyTasks[i]	= NULL;

	/*------------------------------------------------------------------------*/
	/* Initialize all contexts and link them to the free contexts list.*/
	FreeContexts	= Contexts;
	for( i = 0; i < Number - 1; i++, Contexts++ )
		{
		Contexts->Next	= Contexts + 1;
		Contexts->List	= &FreeContexts;
		}
	Contexts->Next		= NULL;
	Contexts->List		= &FreeContexts;

	/*------------------------------------------------------------------------*/
	}
/*============================================================================*/
static void SetupTimerInterrupt( tickcount_t TickRate )
	{
	/* Configure SysTick to interrupt at the requested rate. */

	NVIC_SYST_CVR	= 0ul;
	NVIC_SYST_RVR	= sysclk_get_cpu_hz() / TickRate - 1UL;
	NVIC_SYST_CSR	= NVIC_SYST_CSR_CLKSOURCE | NVIC_SYST_CSR_TICKINT | NVIC_SYST_CSR_ENABLE;
	}
/*============================================================================*/
void __attribute__((noreturn)) StartRTOS( tickcount_t TickRate )
	{
	/* Make CallSV and SysTick the same priroity as the kernel. */
	NVIC_SHPR2	= NVIC_SHPR2_SVCALL_PRI;
	NVIC_SHPR3	= NVIC_SHPR3_SYSTICK_PRI;

	SystemTick	= 0;
	CurrentTask	= ReadyTasks[HighestReadyPriority];

	SetupTimerInterrupt( TickRate );

#if			defined USE_PSP
	__asm volatile(
		"ldr	r0,=CurrentTask			\n"	/* r0	= &CurrentTask	*/
		"ldr	r0,[r0]					\n"	/* r0	= CurrentTask		*/
		"ldr	r1,[r0,#0]				\n"	/* r13	= StackTop		*/
		"msr	psp,r1					\n"

		"mov	r1,2					\n"
		"msr	control,r1				\n"
		"isb							\n"

		"ldr	r1,=0xE000ED08			\n"  /* Use the NVIC offset register to locate the stack. */
		"ldr	r1,[r1]					\n"
		"ldr	r1,[r1]					\n"
		"msr	msp,r1					\n"  /* Set the msp back to the start of the stack. */
		"isb							\n"
		::: "r1"
		);

#else	/*	defined USE_PSP */

	asm volatile(
		"ldr	r0,=CurrentTask			\n"	/* r0	= &CurrentTask	*/
		"ldr	r0,[r0]					\n"	/* r0	= CurrentTask		*/
		"ldr	r13,[r0,#0]				\n"	/* r13	= StackTop		*/
		::: "r0"
		);

#endif	/*	defined USE_PSP */

	asm volatile(
		"ldmia	r13!,{r4-r11}			\n"
		"ldr	r12,[r0,#4]				\n"
		"msr	basepri,r12				\n"
		"ldmia	r13!,{r0-r3,r12,r14}	\n"
		"add	r13,#8					\n"
		"ldr	r15,[r13,-8]			\n"
		::: "r0"
		);

	while( 1 )
		{}
	}
/*============================================================================*/
