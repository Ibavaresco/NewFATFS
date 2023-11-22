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
#include "SimpleRTOSInternals.h"
#include "Timer.h"
/*============================================================================*/
static	srtos_timer_t	*SystemTimers	= NULL;
/*============================================================================*/
static int RemoveTimerFromList( srtos_timer_t *Timer )
	{
	srtos_timer_t	*PreviousTimer, *NextTimer;
	srtos_timer_t	**TimerList;

	if( Timer == NULL || Timer->List == NULL )
		return -1;

	TimerList		= Timer->List;
	PreviousTimer	= Timer->Previous;
	NextTimer		= Timer->Next;

	/* If this Timer is the first one in the list of timers... */
	if( PreviousTimer == NULL )
		/* ... make the next timer in the list (if any) be the first. */
		*TimerList	= NextTimer;
	else
		/* ... else make the timer before this one in the list point to the timer following this one (if any). */
		PreviousTimer->Next	= NextTimer;

	/* If there is a timer after this one in the list... */
	if( NextTimer != NULL )
		/* ... make it point to the timer before this one in the list (if any). */
		NextTimer->Previous	= PreviousTimer;

	/* Mark this timer as not having a previous one in the timers list.*/
	Timer->Previous	= NULL;
	/* Mark this timer as not having a next one in the timers list.*/
	Timer->Next		= NULL;
	/* Mark this timer as not belonging to the timers list.*/
	Timer->List		= NULL;

	return 0;
	}

/*============================================================================*/
static int InsertTimerInList( srtos_timer_t **TimerList, srtos_timer_t *Timer, tickcount_t TickToTrigger )
	{
	srtos_timer_t		*p, *q, *NextTimer;

	if( Timer == NULL || TimerList == NULL )
		return -1;

	/* Set the new timer's time-to-trigger.*/
	Timer->TickToTrigger	= TickToTrigger;
	/* Make the new timer belong to the timers list.*/
	Timer->List		= TimerList;

	/* The list is empty...*/
	if( *TimerList == NULL )
		{
		/* ... just insert the timer into the list.*/
		*TimerList		= Timer;
		/* The list is linear, there is no previous timer.*/
		Timer->Previous	= NULL;
		/* The list is linear, there is no next timer.*/
		Timer->Next		= NULL;
		}
	/* The list is not empty...*/
	else
		{
		tickcount_t	t;

		/* ... make 'p' point to the first element.*/
		p = *TimerList;
		/* Get the first element's time-to-trigger.*/
		t = p->TickToTrigger;
		/* The time-to-trigger of the new timer is less than the first element's...*/
		if( (signed long)( TickToTrigger - t ) < 0 )
			{
			/* ... insert the timer as the first element.*/
			*TimerList		= Timer;
			/* The timer is now the first element, there is no previous one.*/
			Timer->Previous	= NULL;
			/* The former first element is now the next one of this timer.*/
			Timer->Next		= p;
			/* This timer is now the previous one of the former first element.*/
			p->Previous		= Timer;
			}
		/* We need to find where the timer is to be inserted...*/
		else
			{
			/* ... iterate through the remainig elements of the list (if any).*/
			for( q = p->Next; q != NULL; q = q->Next )
				{
				/* Get the time-to-trigger of the timer pointed to by 'q'.*/
				t = q->TickToTrigger;
				/* The time-to-trigger of the new timer is less than the one pointed to by 'q'...*/
				if( (signed long)( TickToTrigger - t ) < 0 )
					/* ... stop.*/
					break;
				/* Advance one element.*/
				p = q;
				}
			/* We are inserting after the element pointed to by 'p'.*/
			NextTimer		= p->Next;
			Timer->Previous	= p;
			Timer->Next		= NextTimer;
			p->Next			= Timer;
			/* There is a next element...*/
			if( NextTimer != NULL )
				/* ... make it point to the new timer.*/
				NextTimer->Previous	= Timer;
			}
		}
	return 0;
	}
/*============================================================================*/
int	StartTimer( srtos_timer_t *Timer, srtos_timer_t **TimerList, tickcount_t Periodicity, unsigned int Cycles, void (*CallBackFunction)( struct srtos_timer_tag * ))
	{
	intsave_t	s;

	if( Timer == NULL || CallBackFunction == NULL || (signed long)Periodicity < 1 || Cycles < 1 )
		return -1;

	Timer->CallBackFunction	= CallBackFunction;
	Timer->Periodicity		= Periodicity;
	Timer->Cycles			= Cycles;

	s	= SaveAndDisableInterrupts();

	InsertTimerInList( TimerList, Timer, GetTickCount() + Periodicity );

	RestoreInterrupts( s );

	return 0;
	}
/*============================================================================*/
int	StartSystemTimer( srtos_timer_t *Timer, tickcount_t Periodicity, unsigned int Cycles, void (*CallBackFunction)( struct srtos_timer_tag * ))
	{
	return StartTimer( Timer, &SystemTimers, Periodicity, Cycles, CallBackFunction );
	}
/*============================================================================*/
int	StopTimer( srtos_timer_t *Timer )
	{
	intsave_t	s;
	int			Status;

	s		= SaveAndDisableInterrupts();

	Status	= RemoveTimerFromList( Timer );

	RestoreInterrupts( s );

	return Status;
	}
/*============================================================================*/
int CheckTimers( srtos_timer_t **TimerList )
	{
	srtos_timer_t	*p;

	if( TimerList == NULL )
		return -1;

    /* The timers list is not empty...*/
    while( ( p = *TimerList ) != NULL && (signed long)( p->TickToTrigger - SystemTick ) <= 0 )
		{
		/* Remove the timer from the timers list.*/
		RemoveTimerFromList( p );

		p->CallBackFunction( p );

		if( p->Cycles + 1 > 1 )
			p->Cycles--;

		if( p->Cycles != 0 )
			/* Insert the timer at the end of the running timers list.*/
			InsertTimerInList( TimerList, p, SystemTick + p->Periodicity );
		}

	if( *TimerList == NULL )
		return 0;
	else
		return 1;
	}
/*============================================================================*/
