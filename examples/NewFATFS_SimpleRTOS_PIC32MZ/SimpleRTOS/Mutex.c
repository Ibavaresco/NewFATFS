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
#include "SimpleRTOSInternals.h"
#include "Mutex.h"
/*============================================================================*/
int MutexInit( mutex_t *Mutex, int Mode )
	{
	if( Mutex == NULL )
		return 0;

	Mutex->Owner		= NULL;
	Mutex->WaitingList	= NULL;
	Mutex->Count		= 0;
	Mutex->Mode			= Mode;

	return 1;
	}
/*============================================================================*/
int MutexTake( mutex_t *Mutex, tickcount_t t )
	{
	register intsave_t	s;

	if( Mutex == NULL )
		return 0;

	s	= SaveAndDisableInterrupts();

	/* The mutex has no owner or is already owned by the current task...*/
	if( Mutex->Owner == NULL || Mutex->Owner == CurrentTask )
		{
		unsigned	n;

		if( Mutex->Owner == NULL )
			{
			Mutex->Count	= 0;
			/* ... make the current task the owner of the mutex.*/
			Mutex->Owner	= CurrentTask;
			}
		n				= ++Mutex->Count;
		/* Return a non-zero value saying that the task got the mutex.*/
		RestoreInterrupts( s );
		return n;
		}
	/* The mutex has a owner already and the task is willing to wait...*/
	else if( t != 0 )
		{
		RemoveTaskFromAllLists( CurrentTask );

		AlreadySwitched	= 1;
		/* Insert the task into the mutex's event list.*/
		InsertTaskInLinearList( CurrentTask, &Mutex->WaitingList );

		/* ... delay or suspend the task (depending on the value of 't').*/
		if( (signed long)t >= 0 )
			/* Insert the task into the delayed tasks list.*/
			InsertTaskInDelayList( CurrentTask, SystemTick + t );
	/*	// This may be useful in the future, but is pointless now.
		// The task is being suspended for undetermined time...
		else
			// ... insert the task into the suspended tasks list.
			InsertTaskInLinearList( CurrentTask, &SuspendedTasks );
	*/
		ForceYield();

		if( Mutex->Owner == CurrentTask )
			{
			Mutex->Count	= 1;
			/* Return a status saying that the task got the mutex.*/
			RestoreInterrupts( s );
			return 1;
			}
		}
	/* Return a status saying that the task didn't get the mutex.*/
	RestoreInterrupts( s );
	return 0;
	}
/*============================================================================*/
int MutexGive( mutex_t *Mutex, int Release )
	{
	register intsave_t	s;
	context_t			*p;
	int					MustYield	= 0;

	if( Mutex == NULL )
		return 0;

	s	= SaveAndDisableInterrupts();

	/* The current task doesn't own the mutex...*/
	if( Mutex->Owner != CurrentTask )
		/* ... return failure.*/
		{
		RestoreInterrupts( s );
		return 0;
		}

	if( Mutex->Count > 1 && Release == 0 )
		{
		RestoreInterrupts( s );
		return --Mutex->Count;
		}
	/* There are tasks awaiting for the mutex...*/
	else if(( p = Mutex->WaitingList ) != NULL )
		{
		/* ... remove the first task from the mutex's event list.*/
		RemoveTaskFromAllLists( p );
		/* Insert the task into the running tasks list.*/
		InsertTaskInReadyTasksList( p );

		if(( Mutex->Mode & MUTEX_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
			MustYield			= 1;

		/* The task is now the owner of the mutex.*/
		Mutex->Count	= 1;
		Mutex->Owner	= p;
		}
	/* There are no tasks waiting for the mutex...*/
	else
		{
		/* ... mark the mutex as having no owner.*/
		Mutex->Count	= 0;
		Mutex->Owner	= NULL;
		}

	if( MustYield )
		ForceYield();

	RestoreInterrupts( s );
	return 1;
	}
/*============================================================================*/
int OwnTheMutex( mutex_t *Mutex )
	{
	register intsave_t	s;

	if( Mutex == NULL )
		return 0;

	s	= SaveAndDisableInterrupts();

	if( Mutex->Owner == CurrentTask )
		{
		RestoreInterrupts( s );
		return 1;
		}
	else
		{
		RestoreInterrupts( s );
		return 0;
		}
	}
/*============================================================================*/

int MutexFlush( mutex_t *Mutex, int Mode )
	{
	register intsave_t	s;
	context_t			*p;
	int					MustYield	= 0;
	int					f	= 0;

	if( Mutex == NULL )
		return 0;

	s	= SaveAndDisableInterrupts();

	if( Mode != 0 && Mutex->Owner == CurrentTask )
		Mutex->Owner	= NULL;

	while(( p = Mutex->WaitingList ) != NULL )
		{
		f	= 1;
		RemoveTaskFromAllLists( p );
		InsertTaskInReadyTasksList( p );

		if(( Mutex->Mode & MUTEX_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
			MustYield			= 1;
		}

	if( MustYield )
		ForceYield();

	RestoreInterrupts( s );

	return f;
	}

/*============================================================================*/
