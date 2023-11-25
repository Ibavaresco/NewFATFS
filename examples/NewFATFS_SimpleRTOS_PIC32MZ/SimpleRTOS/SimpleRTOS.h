/*============================================================================*/
/*
SimpleRTOS - Very simple RTOS for Microcontrollers
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
#if			!defined __SIMPLERTOS_H__
#define __SIMPLERTOS_H__
/*============================================================================*/
#include "Portable.h"
/*============================================================================*/
#if			!defined NULL
#define NULL 0
#endif  /*  !defined NULL */
/*============================================================================*/
#define	QUEUE_STRICT_CHRONOLOGY 1
/*============================================================================*/
void			InitRTOS			( context_t *Contexts, unsigned int Number );

extern void __attribute__((weak)) TickHook( void );
//void			TickHook			( void );

void			Yield				( void );
void			vSleep				( tickcount_t t );
context_t		*CreateTask			( unsigned char *Stack, unsigned long StackSize, void *Parameter, void (*TaskFunc)( void* ), unsigned int Priority, void *FPUContext );
int				ResumeTask			( context_t *Task );
int				DeleteTask			( context_t *Task );
context_t		*GetCurrentTask		( void );
tickcount_t		GetTickCount		( void );
int				ChangeTaskPriority	( context_t *Task, unsigned int NewPriority );
int				RTOSIsRunning		( void );
/*============================================================================*/
#endif  /*  !defined __SIMPLERTOS_H__ */
/*============================================================================*/
