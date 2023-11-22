/*============================================================================*/
/*
SimpleRTOS - Very simple RTOS for Microcontrollers
v2.00 (2014-01-21)
isaacbavaresco@yahoo.com.br
*/
/*============================================================================*/
/*
 Copyright (c) 2007-2017, Isaac Marino Bavaresco
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
#if			!defined __SEMAPHORE_H__
#define __SEMAPHORE_H__
/*============================================================================*/
#include "SimpleRTOS.h"
/*============================================================================*/

/* In this mode, a higher priority task awaken will only run in the next tick */
#define	SEMAPHORE_SWITCH_NORMAL		0

/* In this mode, a higher priority task will run immediately when awaken from another task */
#define	SEMAPHORE_SWITCH_IMMEDIATE	1

/* In this mode, a higher priority task will run immediately when awaken from an ISR */
#define	SEMAPHORE_SWITCH_IN_ISR		2

/*============================================================================*/
typedef struct
	{
	context_t	*WaitingList;
	unsigned	MaximumCount;
	unsigned	Count;
	int			Mode;
	} semaphore_t;
/*============================================================================*/
void	SemaphoreInit		( semaphore_t *Semaphore, unsigned MaximumCount, unsigned InitialCount, int Mode );
int		SemaphoreP			( semaphore_t *Semaphore, unsigned RequestedCount, tickcount_t t );
int		SemaphoreV			( semaphore_t *Semaphore, unsigned Count );
int		SemaphoreVFromISR	( semaphore_t *Semaphore, unsigned Count );
int		SemaphoreFlush		( semaphore_t *Semaphore );
/*============================================================================*/
#endif	/*	!defined __SEMAPHORE_H__ */
/*============================================================================*/
