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
#include "PortPIC32.h"
#include "SimpleRTOSInternals.h"
#include "PortPIC32Internals.h"
/*============================================================================*/
#if			defined __PIC32MK__ || ( defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' )
/*============================================================================*/
fpucontext_t __attribute__((weak))	*FPUHook( context_t * );
context_t							*FPUOwner   = NULL;
/*============================================================================*/
#if         defined __DEBUG
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
#endif  /*  defined __DEBUG */
/*============================================================================*/
void _general_exception_handler( void )
    {
    volatile unsigned int    excep_code;
    /* Mask off Mask of the ExcCode Field from the Cause Register
    Refer to the MIPs Software User's manual */

    excep_code = ( _CP0_GET_CAUSE() & 0x0000007C ) >> 2;

    if( excep_code != 11 )
        {
#if         defined __DEBUG
        static volatile unsigned int excep_addr;
        static volatile const char   *cause_str;

        excep_addr = _CP0_GET_EPC();
        cause_str  = cause[excep_code];

//@@@@        SYS_DEBUG_PRINT( SYS_ERROR_ERROR, "\nGeneral Exception %s (cause=%d, addr=%x).\n", cause_str, excep_code, excep_addr );
		LATGbits.LATG15		= 1;
        while( 1 )
            __asm__ volatile( "sdbbp 0" );
#else   /*  defined __DEBUG */
        while( 1 )
            __asm__ volatile( "nop" );
#endif  /*  defined __DEBUG */
        }

    /* The current task is trying to do FP operation but it doesn't have an
     area to save the FPU context...
    */
    if( CurrentTask->FPUContext == NULL )
        {
        fpucontext_t *FPUContext = NULL;
		
		/* ...we call FPUHook to try and obtain a context for the task... */
        if( FPUHook != NULL && ( FPUContext = FPUHook( CurrentTask )) != NULL )
			{
			/* ...and give that context to the task. */
			CurrentTask->FPUContext = FPUContext;
			}
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
#endif  /*  defined __PIC32MK__ || ( defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' ) */
/*============================================================================*/
