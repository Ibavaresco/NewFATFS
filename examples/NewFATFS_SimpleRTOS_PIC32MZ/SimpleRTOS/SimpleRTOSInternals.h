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
#if			!defined __SIMPLERTOSINTERNALS_H__
#define __SIMPLERTOSINTERNALS_H__
/*============================================================================*/
#include "PortableInternals.h"
/*============================================================================*/
extern int                  RTOSStarted;
extern context_t            *FreeContexts;
extern context_t            *ReadyTasks[];
extern const unsigned int   MaxPriorities;
extern unsigned int         HighestReadyPriority;
extern context_t            *DelayedTasks;
extern context_t            *CurrentTask;
extern tickcount_t          SystemTick;
extern int                  AlreadySwitched;
/*============================================================================*/
void	CheckDelayList				( void );
void	RemoveTaskFromList			( context_t *Task );
void	InsertTaskInCircularList	( context_t *Task, context_t **List );
void    InsertTaskInLinearList      ( context_t *Task, context_t **List );
int		InsertTaskInReadyTasksList	( context_t *Task );
void	InsertTaskInDelayList		( context_t *Task, tickcount_t Time );
void	RemoveTaskFromDelayList		( context_t *Task );
void	RemoveTaskFromAllLists		( context_t *Task );
void    ClearTickInterrupt          ( void );

void    ContextInit                 ( context_t *Context, unsigned char *Stack, unsigned long StackSize, void *Parameter, void (*TaskFunc)( void* ), unsigned int Priority, void *FPUContext );
void	TaskFinish					( void );
/*============================================================================*/
#endif	/*	!defined __SIMPLERTOSINTERNALS_H__ */
/*============================================================================*/
