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
#if			!defined __MUTEX_H__
#define __MUTEX_H__
/*============================================================================*/
#include "SimpleRTOS.h"
/*============================================================================*/

/* In this mode, a higher priority task awaken will only run in the next tick */
#define	MUTEX_NORMAL			0

/* In this mode, a higher priority task will run immediately when awaken from another task */
#define	MUTEX_SWITCH_IMMEDIATE	1

/*============================================================================*/
typedef struct
	{
	context_t	*Owner;
	context_t	*WaitingList;
	unsigned	Count;
	int			Mode;
	} mutex_t;
/*============================================================================*/
int		MutexInit	( mutex_t *Mutex, int Mode );
int		MutexTake	( mutex_t *Mutex, tickcount_t t );
int		MutexGive	( mutex_t *Mutex, int Release );
int		OwnTheMutex	( mutex_t *Mutex );
int		MutexFlush	( mutex_t *Mutex, int Mode );
/*============================================================================*/
#endif	/*	!defined __MUTEX_H__ */
/*============================================================================*/
