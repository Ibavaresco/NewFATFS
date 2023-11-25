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
#include "Semaphore.h"
/*============================================================================*/
void SemaphoreInit( semaphore_t *Semaphore, unsigned MaximumCount, unsigned InitialCount, int Mode )
	{
	if( InitialCount > MaximumCount )
		InitialCount	= MaximumCount;

	Semaphore->WaitingList	= NULL;
	Semaphore->MaximumCount	= MaximumCount;
	Semaphore->Count		= InitialCount;
	Semaphore->Mode			= Mode;
	}
/*============================================================================*/
int SemaphoreP( semaphore_t *Semaphore, unsigned RequestedCount, tickcount_t t )
	{
	register intsave_t	s;

	/* The semaphore is invalid or the caller is requesting zero counts... */
	if( Semaphore == NULL || RequestedCount == 0 )
		/* ... there's nothing to be done. */
		return 0;

	/* The caller has requested an invalid number of counts... */
	if(( Semaphore->MaximumCount == 0 && RequestedCount > 1 ) || ( Semaphore->MaximumCount > 0 && RequestedCount > Semaphore->MaximumCount ))
		/* ... return an error status. */
		return -1;

	s	= SaveAndDisableInterrupts();

	/* There are enough counts available for the request... */
	if( RequestedCount <= Semaphore->Count )
		/* ... just decrease the available counts and let the caller have them. */
		Semaphore->Count   -= RequestedCount;
	/* There are not enough counts available for the request and the caller is not willing to wait... */
	else if( t == 0 )
		/* ... simply return denying the counts to the caller. */
		RequestedCount		= 0;
	/* There are not enough counts available for the request but the caller is willing to wait... */
	else
		{
		/* Remove the task from the list of running tasks. */
		RemoveTaskFromAllLists( CurrentTask );

		/* Store the requested counts in the task's context so it will be accessible when needed. */
		CurrentTask->ExtraParameter	= (void*)RequestedCount;

		/* The task that will be switched-in will run for the remaining time of this tick, but it will have its own tick next. */
		AlreadySwitched	= 1;

		/* Insert the task into the semaphore's waiting list.*/
		InsertTaskInLinearList( CurrentTask, &Semaphore->WaitingList );

		/* ... delay or suspend the task (depending on the value of 't').*/
		if( (signed long)t >= 0 )
			/* Insert the task into the delayed tasks list.*/
			InsertTaskInDelayList( CurrentTask, GetTickCount() + t );
		/*
		// This may be useful in the future, but is pointless now.
		// The task is being suspended for undetermined time...
		else
			// ... insert the task into the suspended tasks list.
			InsertTaskInLinearList( CurrentTask, &SuspendedTasks );
		*/

		/* Yields control to the next task. */
		ForceYield();

		/*
		Execution will resume here only when the task gets the semaphore counts
		it requested or if the time to wait has elapsed.
		*/

		/* ExtraParameter is not zero, meaning that the task didn't get the counts it requested... */
		if( CurrentTask->ExtraParameter != 0 )
			{
			CurrentTask->ExtraParameter	= 0;
			/* Return zero to signal that the caller didn't get the semaphore. */
			RequestedCount				= 0;
			}
		}

	RestoreInterrupts( s );

	/* Return the number of counts that the caller has got. */
	return RequestedCount;
	}
/*============================================================================*/
#if			!defined min
static inline int __attribute__((always_inline)) min( int a, int b )
	{
	return a < b ? a : b;
	}
#endif	/*	!defined min */
/*============================================================================*/
int SemaphoreV( semaphore_t *Semaphore, unsigned Count )
	{
	register intsave_t	s;
	context_t			*p;
	int					MustYield	= 0;

	if( Semaphore == NULL )
		return -1;
/*
	if( Semaphore->MaximumCount > 0 && Count > Semaphore->MaximumCount || Semaphore->MaximumCount == 0 && Count > 1 )
		return -1;
*/
	s	= SaveAndDisableInterrupts();

	/* There are tasks awaiting for the semaphore...*/
	for( Count += Semaphore->Count; ( p = Semaphore->WaitingList ) != NULL && Count >= (unsigned)( p->ExtraParameter );
			Count -= (unsigned)( p->ExtraParameter ))
		{
		/* ... remove the first task from the semaphore's event list.*/
		RemoveTaskFromAllLists( p );
		/* Insert the task into the running tasks list.*/
		InsertTaskInReadyTasksList( p );

		/* Zero the task's extra parameter to signal that the task got the count it requested. */
		p->ExtraParameter	= 0;

		if(( Semaphore->Mode & SEMAPHORE_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
			MustYield	= 1;
		}

	if( Count > 0 && Semaphore->MaximumCount > 0 )
		Semaphore->Count	= min( Semaphore->Count + Count, Semaphore->MaximumCount );

	if( MustYield )
		ForceYield();

	RestoreInterrupts( s );
	return 1;
	}
/*============================================================================*/
int SemaphoreVFromISR( semaphore_t *Semaphore, unsigned Count )
	{
	context_t	*p;
	int			Result	= 0;

	if( Semaphore == NULL )
		return -1;

	/* There are tasks awaiting for the semaphore...*/
	for( Count += Semaphore->Count; ( p = Semaphore->WaitingList ) != NULL && Count >= (unsigned)( p->ExtraParameter );
			Count -= (unsigned)( p->ExtraParameter ))
		{
		/* ... remove the first task from the semaphore's event list.*/
		RemoveTaskFromAllLists( p );
		/* Insert the task into the running tasks list.*/
		InsertTaskInReadyTasksList( p );

		p->ExtraParameter	= 0;

		if( Semaphore->Mode & SEMAPHORE_SWITCH_IN_ISR )
			Result	= 2;
		else if( Result == 0 )
			Result	= 1;
		}

	return Result;
	}
/*============================================================================*/
int SemaphoreFlush( semaphore_t *Semaphore )
	{
	register intsave_t	s;
	context_t			*p;
	int					MustYield	= 0;
	int					TasksAwakened;

	if( Semaphore == NULL )
		return -1;

	s	= SaveAndDisableInterrupts();

	for( TasksAwakened	= 0; ( p = Semaphore->WaitingList ) != NULL; TasksAwakened++ )
		{
		RemoveTaskFromAllLists( p );
		InsertTaskInReadyTasksList( p );

		if(( Semaphore->Mode & SEMAPHORE_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
			MustYield			= 1;
		}

	if( MustYield )
		ForceYield();

	RestoreInterrupts( s );

	return TasksAwakened;
	}
/*============================================================================*/
