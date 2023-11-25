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
#include <string.h>
#include <xc.h>                 /* Defines special funciton registers, CP0 regs  */
#if			defined __DEBUG && ( defined __PIC32MK__ || ( defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' ))
//#include "system_config.h"
//#include "system_definitions.h"
//#include "system/debug/sys_debug.h"
#endif	/*	defined __DEBUG && ( defined __PIC32MK__ || ( defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' )) */

#include "PortPIC32.h"
#include "SimpleRTOSInternals.h"
#include "PortPIC32Internals.h"
/*============================================================================*/
extern void __attribute__((weak)) TickHook( void );
/*============================================================================*/
#if         !defined USE_CORE_TIMER || USE_CORE_TIMER == 0
void __attribute__(( vector( _TIMER_1_VECTOR ), interrupt( IPL1SOFT ), nomips16 )) TickInterrupt( void );
#else	/*	!defined USE_CORE_TIMER || USE_CORE_TIMER == 0 */
void __attribute__(( vector( _CORE_TIMER_VECTOR ), interrupt( IPL1SOFT ), nomips16 )) TickInterrupt( void );
static uint32_t	CTCompareReload	= 0;
#endif	/*	!defined USE_CORE_TIMER || USE_CORE_TIMER == 0 */

int SchedulerSuspended  = 0;
/*============================================================================*/
int SuspendScheduler( void )
    {
    int Aux;
    
    asm volatile( "di" );
    asm volatile( "ehb" );
    Aux = SchedulerSuspended;
    SchedulerSuspended  = 1;
    asm volatile( "ei" );
    return Aux;
    }
/*============================================================================*/
int ResumeScheduler( void )
    {
    int Aux;
    
    asm volatile( "di" );
    asm volatile( "ehb" );
    Aux = SchedulerSuspended;
    SchedulerSuspended  = 0;
    asm volatile( "ei" );
    return Aux;
    }
/*============================================================================*/
int SchedulerIsSuspended( void )
    {
    return SchedulerSuspended;
    }
/*============================================================================*/
void ContextInit( context_t *Context, unsigned char *Stack, unsigned long StackSize, void *Parameter, void (*TaskFunc)( void* ), unsigned int Priority, void *FPUContext )
	{
	memset( Context, 0x00, sizeof( context_t ));

	Context->r4			= (unsigned long)Parameter;
	/*Context->r28		= $gp;*/
	asm volatile( "sw	$gp,%0" :: "m"( Context->r28 ));
	Context->r29		= ( (unsigned long)Stack + StackSize - 4 ) & ~7;
	Context->r30		= ( (unsigned long)Stack + StackSize - 4 ) & ~7;
	Context->r31		= (unsigned long)TaskFinish;
	Context->CP0_EPC	= (unsigned long)TaskFunc;

#if         defined __PIC32MK__ || ( defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' )
    Context->FPUContext = FPUContext;
#endif  /*  defined __PIC32MK__ || ( defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' ) */

#if			defined __PIC32MK__ || defined __PIC32MZ__
	#if			defined __PIC32MK__ || __PIC32_FEATURE_SET1 == 'F'
	Context->CP0_STATUS	= 0x05000003;
	#else	/*	defined __PIC32MK__ || __PIC32_FEATURE_SET1 == 'F' */
	Context->CP0_STATUS	= 0x01000003;
	#endif	/*	defined __PIC32MK__ || __PIC32_FEATURE_SET1 == 'F' */
#elif		defined __PIC32MX__
	Context->CP0_STATUS	= 0x00000003;
#endif	/*	defined __PIC32MK__ || defined __PIC32MZ__ */
	Context->CP0_CAUSE	= _CP0_GET_CAUSE();

	Context->Priority				= Priority;
	Context->TDelay					= 0;
	Context->PreviousDelayedTask	= 0;
	Context->NextDelayedTask		= 0;
	Context->DelayList				= 0;

#ifdef		USE_SLICE_LENGTH
	Context->SliceLength            = 0;
#endif	/*	USE_SLICE_LENGTH */

	Context->Previous				= 0;
	Context->Next					= 0;
	Context->List					= 0;
	}
/*============================================================================*/
void ClearTickInterrupt( void )
	{
#if         !defined USE_CORE_TIMER || USE_CORE_TIMER == 0
	IFS0bits.T1IF	= 0;
#else	/*	!defined USE_CORE_TIMER || USE_CORE_TIMER == 0 */
	uint32_t	CT, CP;

	CT	= _CP0_GET_COUNT();
	CP	= _CP0_GET_COMPARE();
	if( (uint32_t)( CT - CP ) < (uint32_t)( CTCompareReload - 1000ul ))
		_CP0_SET_COMPARE( CP + CTCompareReload );
	else
		{
		_CP0_SET_COUNT( 0 );
		_CP0_SET_COMPARE( CTCompareReload );
		}
	IFS0bits.CTIF	= 0;
#endif	/*	!defined USE_CORE_TIMER || USE_CORE_TIMER == 0 */
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
void __attribute__((nomips16,noreturn)) StartRTOS( tickcount_t TickPeriod )
	{
#if         !defined USE_CORE_TIMER || USE_CORE_TIMER == 0
	int	PS;

	if( TickPeriod <= 65536 )
		{
		PS			 = 0;
		}
	else if( TickPeriod <= 65536 * 8 )
		{
		TickPeriod >>= 3;
		PS			 = 1;
		}
	else if( TickPeriod <= 65536 * 64 )
		{
		TickPeriod >>= 6;
		PS			 = 2;
		}
	else
		{
		TickPeriod >>= 8;
		PS			 = 3;
		}

	/* set up the core timer interrupt with a prioirty of 2 and zero sub-priority */
	/*INTSetVectorPriority( INT_TIMER_1_VECTOR, configKERNEL_INTERRUPT_PRIORITY );*/
	IPC1bits.T1IP	= KERNEL_INTERRUPT_PRIORITY;
	/*INTSetVectorSubPriority( INT_TIMER_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0 );*/
	IPC1bits.T1IS	= 0;
	/*INTClearFlag( INT_T1 );*/
	IFS0bits.T1IF	= 0;
	/*INTEnable( INT_T1, INT_ENABLED );*/
	IEC0bits.T1IE	= 1;
#endif	/*	!defined USE_CORE_TIMER || USE_CORE_TIMER == 0 */

	/* configure for multi-vectored mode*/
	/*INTConfigureSystem( INT_SYSTEM_CONFIG_MULT_VECTOR );*/
	INTCONbits.MVEC = 1;

	SystemTick		= 0;
	CurrentTask		= ReadyTasks[HighestReadyPriority];

#if         !defined USE_CORE_TIMER || USE_CORE_TIMER == 0
	/*OpenTimer1( T1_ON | PS, TickPeriod );*/	/* load with the period */
	PR1				= TickPeriod-1;
	TMR1			= 0;
	T1CONbits.TCKPS = PS;
	T1CONbits.TON	= 1;
#else	/*	!defined USE_CORE_TIMER || USE_CORE_TIMER == 0 */
	CTCompareReload	= TickPeriod / 2;
	IPC0bits.CTIP	= KERNEL_INTERRUPT_PRIORITY;
	IPC0bits.CTIS	= 0;
	_CP0_SET_COMPARE( CTCompareReload );
	_CP0_SET_COUNT( 0 );
	IFS0bits.CTIF	= 0;
	IEC0bits.CTIE	= 1;
#endif	/*	!defined USE_CORE_TIMER || USE_CORE_TIMER == 0 */

	/* enable interrupts*/
	/*INTEnableInterrupts();*/
	_CP0_BIC_STATUS( 0x07 << 10 );
#if			defined __PIC32MK__ || defined __PIC32MZ__
	_CP0_BIS_STATUS( 0x01 << 24 );
#endif	/*	defined __PIC32MK__ || defined __PIC32MZ__ */

	asm volatile(
		".extern	RestoreContext	\n"
		".set		noreorder		\n"
		".set 		noat			\n"

		"addiu		$sp,$sp,-8		\n"
		"sw			$ra,4($sp)		\n"

		"j			RestoreContext	\n"
		"nop						\n"
		);

	while( 1 )
		{}
	}
/*============================================================================*/
