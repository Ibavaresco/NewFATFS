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
#if			!defined __TIMER_H__
#define __TIMER_H__
/*============================================================================*/
#include "SimpleRTOS.h"
/*============================================================================*/
typedef struct srtos_timer_tag
	{
	struct	srtos_timer_tag	**List;
	struct	srtos_timer_tag	*Next;
	struct	srtos_timer_tag	*Previous;
	tickcount_t			TickToTrigger;
	tickcount_t			Periodicity;
	unsigned int		Cycles;
	void				(*CallBackFunction)( struct srtos_timer_tag * );
	} srtos_timer_t;
/*============================================================================*/
int StartTimer          ( srtos_timer_t *Timer, srtos_timer_t **TimerList, tickcount_t Periodicity, unsigned int Cycles, void (*CallBackFunction)( struct srtos_timer_tag * ));
int StartSystemTimer    ( srtos_timer_t *Timer, tickcount_t Periodicity, unsigned int Cycles, void (*CallBackFunction)( struct srtos_timer_tag * ));
int StopTimer           ( srtos_timer_t *Timer );
int CheckTimers         ( srtos_timer_t **TimerList );
/*============================================================================*/
#endif	/*	!defined __TIMER_H__ */
/*============================================================================*/
