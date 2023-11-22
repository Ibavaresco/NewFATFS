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
#if			defined __XC32__  || defined __C32_VERSION__
/*============================================================================*/
#include <xc.h>                 /* Defines special function registers, CP0 regs  */
#include <string.h>
//#include "system_config.h"
//#include "system_definitions.h"
//#include "system/debug/sys_debug.h"

#include "PortPIC32.h"
#include "SimpleRTOSInternals.h"
#include "PortPIC32Internals.h"
/*============================================================================*/
extern void __attribute__((weak)) TickHook( void );
/*============================================================================*/
//void __attribute__(( vector( _TIMER_1_VECTOR ), interrupt( IPL1SOFT ), nomips16 ))T1Interrupt( void );
void __attribute__(( vector( _CORE_TIMER_VECTOR ), interrupt( IPL1SOFT ), nomips16 )) CTInterrupt( void );
/*============================================================================*/
static unsigned long	CTCompareReload	= 0;
/*============================================================================*/
void Switch( void )
	{
	/*INTClearFlag( INT_T1 );*/ 	/* clear the interrupt flag */
	//IFS0bits.T1IF	= 0;

	_CP0_SET_COMPARE( _CP0_GET_COMPARE() + CTCompareReload );
	IFS0bits.CTIF	= 0;

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

#if         defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F'
context_t   *FPUOwner   = NULL;
#endif  /*  defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' */

/*============================================================================*/
void ContextInit( context_t *Context, unsigned char *Stack, unsigned long StackSize, void *Parameter, void (*TaskFunc)( void* ), unsigned int Priority, void *FPUContext )
	{
	unsigned long	Aux;
	unsigned long	*p	= (unsigned long*)(( (unsigned long)Stack + StackSize ) & ~7 ) - 2;

	memset( Context, 0x00, sizeof( context_t ));

	*--p	= (unsigned long)TaskFinish;	/* $31 = ra */
	*--p	= 0x00000000;					/* $16 = s0 */
#if			defined __PIC32MZ__
	*--p	= 0x00000000;					/* dsp		*/
	*--p	= 0x00000000;					/* ac3h		*/
	*--p	= 0x00000000;					/* ac3l		*/
	*--p	= 0x00000000;					/* ac2h		*/
	*--p	= 0x00000000;					/* ac2l		*/
	*--p	= 0x00000000;					/* ac1h		*/
	*--p	= 0x00000000;					/* ac1l		*/
#elif		defined __PIC32MX__
	*--p	= 0x00000000;					/* Dummy	*/
#endif	/*	defined __PIC32MZ__ */
	*--p	= 0x00000000;					/* hi		*/
	*--p	= 0x00000000;					/* lo		*/
	*--p	= (unsigned long)TaskFunc;		/* EPC		*/
#if			defined __PIC32MZ__
	#if			__PIC32_FEATURE_SET1 == 'F'
	*--p	= 0x05000003;					/* STATUS	*/
	#else	/*	__PIC32_FEATURE_SET1 == 'F' */
	*--p	= 0x01000003;					/* STATUS	*/
	#endif	/*	__PIC32_FEATURE_SET1 == 'F' */
#elif		defined __PIC32MX__
	*--p	= 0x00000003;					/* STATUS	*/
#endif	/*	defined __PIC32MZ__ */
	*--p	= 0x00000000;					/* errno	*/
	asm volatile( "sw	$gp,%0" :: "m"( Aux ));
	*--p	= Aux;							/* $28 = gp	*/
	*--p	= 0x00000000;					/* $27 = k1	*/
	*--p	= 0x00000000;					/* $26 = k0	*/
	*--p	= 0x00000000;					/* $25 = t9	*/
	*--p	= 0x00000000;					/* $24 = t8	*/
	*--p	= 0x00000000;					/* $23 = s7	*/
	*--p	= 0x00000000;					/* $22 = s6	*/
	*--p	= 0x00000000;					/* $21 = s5	*/
	*--p	= 0x00000000;					/* $20 = s4	*/
	*--p	= 0x00000000;					/* $19 = s3	*/
	*--p	= 0x00000000;					/* $18 = s2	*/
	*--p	= 0x00000000;					/* $17 = s1	*/
	*--p	= ( (unsigned long)Stack + StackSize ) & ~7;	/* $30 = fp	*/
	*--p	= 0x00000000;					/* $15 = t7	*/
	*--p	= 0x00000000;					/* $14 = t6	*/
	*--p	= 0x00000000;					/* $13 = t5	*/
	*--p	= 0x00000000;					/* $12 = t4	*/
	*--p	= 0x00000000;					/* $11 = t3	*/
	*--p	= 0x00000000;					/* $10 = t2	*/
	*--p	= 0x00000000;					/*  $9 = t1	*/
	*--p	= 0x00000000;					/*  $8 = t0	*/
	*--p	= 0x00000000;					/*  $7 = a3	*/
	*--p	= 0x00000000;					/*  $6 = a2	*/
	*--p	= 0x00000000;					/*  $5 = a1	*/
	*--p	= (unsigned long)Parameter;		/*  $4 = a0	*/
	*--p	= 0x00000000;					/*  $3 = v1	*/
	*--p	= 0x00000000;					/*  $2 = v0	*/
	*--p	= 0x00000000;					/*  $1 = at	*/

	Context->sp	= (unsigned long)p;

#if         defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F'
    Context->FPUContext = FPUContext;
#endif  /*  defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' */

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
	/* configure for multi-vectored mode*/
	/*INTConfigureSystem( INT_SYSTEM_CONFIG_MULT_VECTOR );*/
	INTCONbits.MVEC = 1;

	SystemTick	= 0;
	CurrentTask	= ReadyTasks[HighestReadyPriority];

	/*OpenTimer1( T1_ON | PS, TickPeriod );*/	/* load with the period */
#if 0
	PR1				= TickPeriod-1;
	TMR1			= 0;
	T1CONbits.TCKPS = PS;
	T1CONbits.TON	= 1;
#else
	CTCompareReload	= TickPeriod;
	_CP0_SET_COUNT( 0 );
	_CP0_SET_COMPARE( CTCompareReload );
	IPC0bits.CTIP	= KERNEL_INTERRUPT_PRIORITY;
	IPC0bits.CTIS	= 0;
	IFS0bits.CTIF	= 0;
	IEC0bits.CTIE	= 1;
#endif

	RTOSStarted		= 1;
	/* enable interrupts*/
	/*INTEnableInterrupts();*/
	_CP0_BIC_STATUS( 0x07 << 10 );
#if			defined __PIC32MZ__
	_CP0_BIS_STATUS( 0x01 << 24 );
#endif	/*	defined __PIC32MZ__ */

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
#endif	/*	defined __XC32__  || defined __C32_VERSION__ */
/*============================================================================*/

/*============================================================================*/
#if     defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F'
/*============================================================================*/

void    SaveFP      ( fpucontext_t * );
void    RestoreFP   ( fpucontext_t * );

/*============================================================================*/
void    RestoreContext( void );
/*============================================================================*/
fpucontext_t    __attribute__((weak)) *FPUHook( context_t * );
/*============================================================================*/

static const char *cause[] = 
    {
    "Interrupt",
    "Undefined",
    "Undefined",
    "Undefined",
    "Load/fetch address error",
    "Store address error",
    "Instruction bus error",
    "Data bus error",
    "Syscall",
    "Breakpoint",
    "Reserved instruction",
    "Coprocessor unusable",
    "Arithmetic overflow",
    "Trap",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
    };

#define SYS_DEBUG_PRINT(...)

void _general_exception_handler ( void )
    {
    volatile unsigned int    _excep_code;
    /* Mask off Mask of the ExcCode Field from the Cause Register
    Refer to the MIPs Software User's manual */

    _excep_code = ( _CP0_GET_CAUSE() & 0x0000007C ) >> 2;

    if( _excep_code != 11 )
        {
        static unsigned int _excep_addr;
        static const char   *_cause_str;

        _excep_addr = _CP0_GET_EPC();
        _cause_str  = cause[_excep_code];

        SYS_DEBUG_PRINT( SYS_ERROR_ERROR, "\nGeneral Exception %s (cause=%d, addr=%x).\n", 
                        _cause_str, _excep_code, _excep_addr );
        while( 1 )
            __asm__ volatile (" sdbbp 0");
        }

    /* The current task is trying to do FP operation but it doesn't have an
     area to save the FPU context...
    */
    if( CurrentTask->FPUContext == NULL )
        {
        fpucontext_t *FPUContext;

		/* ...we call FPUHook to try and obtain a context for the task... */
        if( FPUHook != NULL && ( FPUContext = FPUHook( CurrentTask )) != NULL )
			/* ...and give that context to the task. */
            CurrentTask->FPUContext = FPUContext;
        else
            {
			/* ...but either FPUHook is not defined or it didn't give us an FPU context... */
            /* so we are going to kill the task. */
            RemoveTaskFromAllLists( CurrentTask );

            /* Link the task's context to the free contexts list*/
            CurrentTask->Next   = FreeContexts;
            FreeContexts        = CurrentTask;
            /* Mark the context as belonging to the free contexts list.*/
            CurrentTask->List   = &FreeContexts;

            CurrentTask         = ReadyTasks[HighestReadyPriority];
            RestoreContext();
            }
        }

	/* We must enable CU1 before we do any FPU operations ourselves. */
    _CP0_SET_STATUS( _CP0_GET_STATUS() | 0x20000000 );

	/* The current task doesn't own the FPU... */
    if( FPUOwner != CurrentTask )
        {
		/* ...but other task does... */
        if( FPUOwner != NULL )
            {
			/* ...so we need to save its FPU context... */
            SaveFP( FPUOwner->FPUContext );
			/* ...and mark the FPU as unowned. */
            FPUOwner    = NULL;
            }

		/* ...then we restore the current task's FPU context... */
        RestoreFP( CurrentTask->FPUContext );
		/* ...and mark the FPU as being owned by the current task. */
        FPUOwner        = CurrentTask;
        }
    }

/*============================================================================*/

fpucontext_t *GetTaskFPUContext( context_t *Task )
    {
    if( Task == NULL )
        Task    = CurrentTask;

    return Task->FPUContext;
    }

/*============================================================================*/

int SetTaskFPUContext( context_t *Task, fpucontext_t *FPUContext )
    {
    intsave_t       s   = SaveAndDisableInterrupts();
    int             Aux = 0;

    if( Task == NULL )
        Task    = CurrentTask;

    if( FPUContext == NULL )
        {
        if( Task == CurrentTask )
            _CP0_SET_STATUS( _CP0_GET_STATUS() & ~0x20000000 );
        Task->FPUContext    = NULL;
        }
    else if( Task->FPUContext == NULL )
        Task->FPUContext    = FPUContext;
    else
        Aux = -1;

    RestoreInterrupts( s );

    return Aux;
    }

/*============================================================================*/
#endif  /*  defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' */
/*============================================================================*/
