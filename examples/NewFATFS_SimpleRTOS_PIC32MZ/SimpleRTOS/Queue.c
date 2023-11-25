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
#include <string.h>
#include "SimpleRTOSInternals.h"
#include "Queue.h"
/*============================================================================*/
void QueueInit( queue_t *Queue, int ItemSize, int QueueLength, void *QueueStart, int Mode )
	{
#if			defined QUEUE_STRICT_CHRONOLOGY
	Queue->ReadingOwner			= NULL;
	Queue->WritingOwner			= NULL;
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
	Queue->TasksWaitingToWrite	= NULL;
	Queue->TasksWaitingToRead	= NULL;
	Queue->ItemSize				= ItemSize;
	Queue->QueueLength			= QueueLength;
	Queue->QueueStart			= (unsigned char*)QueueStart;
	Queue->ItemsAvailable		= 0;
	Queue->ItemToRemove			= 0;
	Queue->Mode					= Mode;
	}
/*============================================================================*/
int QueueRead( queue_t *Queue, void *Ptr, tickcount_t TimeToWait )
	{
	register intsave_t	s;
	context_t			*p;
	int					MustYield = 0;

	if( Queue == NULL )
		return 0;

	s	= SaveAndDisableInterrupts();

#if			defined QUEUE_STRICT_CHRONOLOGY
	if( Queue->ItemsAvailable == 0 || Queue->ReadingOwner != NULL || Queue->TasksWaitingToRead != NULL )
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
	if( Queue->ItemsAvailable == 0 )
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
		{
		tickcount_t	DeadLine;

		if( TimeToWait == 0 )
			{
			RestoreInterrupts( s );
			return 0;
			}
	
		DeadLine	= GetTickCount() + TimeToWait;

#if			!defined QUEUE_STRICT_CHRONOLOGY
		do
#endif	/*	!defined QUEUE_STRICT_CHRONOLOGY */
			{
			/* Remove the task from the running tasks list */
			RemoveTaskFromAllLists( CurrentTask );

			InsertTaskInLinearList( CurrentTask, &Queue->TasksWaitingToRead );
			AlreadySwitched		= 1;
			if( (signed long)TimeToWait >= 0 )
				InsertTaskInDelayList( CurrentTask, DeadLine );
			ForceYield();
			}
#if			!defined QUEUE_STRICT_CHRONOLOGY
		while(( (signed long)TimeToWait < 0 || (signed long)( DeadLine - GetTickCount() ) > 0 ) && Queue->ItemsAvailable == 0 );
#endif	/*	!defined QUEUE_STRICT_CHRONOLOGY */

#if			defined QUEUE_STRICT_CHRONOLOGY
		if( Queue->ItemsAvailable == 0 || Queue->ReadingOwner != CurrentTask )
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
		if( Queue->ItemsAvailable == 0 )
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
			{
			RestoreInterrupts( s );
			return 0;
			}
		}

    memcpy( (void*)Ptr, (void*)&Queue->QueueStart[ Queue->ItemToRemove * Queue->ItemSize ], Queue->ItemSize );
    if( ++Queue->ItemToRemove >= Queue->QueueLength )
    	Queue->ItemToRemove	= 0;
    Queue->ItemsAvailable--;

#if         defined QUEUE_STRICT_CHRONOLOGY
    Queue->ReadingOwner		= NULL;

    if( Queue->ItemsAvailable != 0 && ( p = Queue->TasksWaitingToRead ) != NULL )
        {
		Queue->ReadingOwner	= p;
        /* ... remove the first task from the queue's event list. */
		RemoveTaskFromAllLists( p );
        /* Insert the task into the running tasks list. */
		InsertTaskInReadyTasksList( p );

		if(( Queue->Mode & QUEUE_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
        	MustYield	= 1;
		}

    if( Queue->WritingOwner == NULL && ( p = Queue->TasksWaitingToWrite ) != NULL )
        {
        Queue->WritingOwner	= p;
#else	/*  defined QUEUE_STRICT_CHRONOLOGY */
        if(( p = Queue->TasksWaitingToWrite ) != NULL )
            {
#endif	/*  defined QUEUE_STRICT_CHRONOLOGY */
            /* ... remove the first task from the queue's event list. */
		RemoveTaskFromAllLists( p );
        /* Insert the task into the running tasks list. */
		InsertTaskInReadyTasksList( p );

		if(( Queue->Mode & QUEUE_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
        	MustYield	= 1;
        }

    if( MustYield )
        ForceYield();
    RestoreInterrupts( s );
    return 1;
    }
/*============================================================================*/
int QueueReadFromISR( queue_t *Queue, void *Ptr )
	{
	context_t		*p;

	if( Queue == NULL )
		return 0;

#if			defined QUEUE_STRICT_CHRONOLOGY
	if( Queue->ItemsAvailable == 0 || Queue->ReadingOwner != NULL || Queue->TasksWaitingToRead != NULL )
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
	if( Queue->ItemsAvailable == 0 )
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
		return 0;

	memcpy( (void*)Ptr, (void*)&Queue->QueueStart[ Queue->ItemToRemove * Queue->ItemSize ], Queue->ItemSize );
	if( ++Queue->ItemToRemove >= Queue->QueueLength )
		Queue->ItemToRemove	= 0;
	Queue->ItemsAvailable--;

#if			defined QUEUE_STRICT_CHRONOLOGY
	if( Queue->WritingOwner == NULL && ( p = Queue->TasksWaitingToWrite ) != NULL )
		{
		Queue->WritingOwner	= p;
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
	if(( p = Queue->TasksWaitingToWrite ) != NULL )
		{
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
        /* ... remove the first task from the queue's event list. */
		RemoveTaskFromAllLists( p );
        /* Insert the task into the running tasks list. */
		if( InsertTaskInReadyTasksList( p ))
			{
			if( Queue->Mode & QUEUE_SWITCH_IN_ISR )
				return 2;
			}
		}

	return 1;
	}
/*============================================================================*/
int QueueWrite( queue_t *Queue, const void *Ptr, tickcount_t TimeToWait )
	{
	register intsave_t	s;
	context_t			*p;
	unsigned int		i;
	int					MustYield	= 0;

	if( Queue == NULL )
		return 0;

	s	= SaveAndDisableInterrupts();

#if			defined QUEUE_STRICT_CHRONOLOGY
	if( Queue->ItemsAvailable >= Queue->QueueLength || Queue->WritingOwner != NULL || Queue->TasksWaitingToWrite != NULL )
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
	if( Queue->ItemsAvailable >= Queue->QueueLength )
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
		{
		tickcount_t	DeadLine;

		if( TimeToWait == 0 )
			{
			RestoreInterrupts( s );
			return 0;
			}

		DeadLine	= GetTickCount() + TimeToWait;

#if			!defined QUEUE_STRICT_CHRONOLOGY
		do
#endif	/*	!defined QUEUE_STRICT_CHRONOLOGY */
			{
			/* Remove the task from the running tasks list */
			RemoveTaskFromAllLists( CurrentTask );

			InsertTaskInLinearList( CurrentTask, &Queue->TasksWaitingToWrite );
		    AlreadySwitched		= 1;
		    if( (signed long)TimeToWait >= 0 )
	        	InsertTaskInDelayList( CurrentTask, DeadLine );
	        ForceYield();
	        }
#if			!defined QUEUE_STRICT_CHRONOLOGY
		while(( (signed long)TimeToWait < 0 || (signed long)( DeadLine - GetTickCount() ) > 0 ) && Queue->ItemsAvailable >= Queue->QueueLength );
#endif	/*	!defined QUEUE_STRICT_CHRONOLOGY */


#if			defined QUEUE_STRICT_CHRONOLOGY
		if( Queue->ItemsAvailable >= Queue->QueueLength || Queue->WritingOwner != CurrentTask )
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
		if( Queue->ItemsAvailable >= Queue->QueueLength )
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
			{
			RestoreInterrupts( s );
			return 0;
			}
		}

	if(( i = Queue->ItemToRemove + Queue->ItemsAvailable ) >= Queue->QueueLength )
		i	-= Queue->QueueLength;
	memcpy( &Queue->QueueStart[ i * Queue->ItemSize ], Ptr, Queue->ItemSize );
	Queue->ItemsAvailable++;

#if			defined QUEUE_STRICT_CHRONOLOGY
	Queue->WritingOwner		= NULL;

	if( Queue->ItemsAvailable < Queue->QueueLength && ( p = Queue->TasksWaitingToWrite ) != NULL )
		{
		Queue->WritingOwner	= p;
        /* ... remove the first task from the queue's event list. */
		RemoveTaskFromAllLists( p );
        /* Insert the task into the running tasks list. */
		InsertTaskInReadyTasksList( p );

		if(( Queue->Mode & QUEUE_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
        	MustYield	= 1;
		}

	if( Queue->ReadingOwner == NULL && ( p = Queue->TasksWaitingToRead ) != NULL )
		{
		Queue->ReadingOwner	= p;
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
	if(( p = Queue->TasksWaitingToRead ) != NULL )
		{
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
        /* ... remove the first task from the queue's event list. */
		RemoveTaskFromAllLists( p );
        /* Insert the task into the running tasks list. */
		InsertTaskInReadyTasksList( p );

		if(( Queue->Mode & QUEUE_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
        	MustYield	= 1;
		}

	if( MustYield )
		ForceYield();

	RestoreInterrupts( s );
	return 1;
	}
/*============================================================================*/
int QueueWriteFromISR( queue_t *Queue, const void *Ptr )
	{
	context_t		*p;
	unsigned int	i;

	if( Queue == NULL )
		return 0;

#if			defined QUEUE_STRICT_CHRONOLOGY
	if( Queue->ItemsAvailable >= Queue->QueueLength || Queue->WritingOwner != NULL || Queue->TasksWaitingToWrite != NULL )
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
	if( Queue->ItemsAvailable >= Queue->QueueLength )
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
		return 0;

	if(( i = Queue->ItemToRemove + Queue->ItemsAvailable ) >= Queue->QueueLength )
		i	-= Queue->QueueLength;
	memcpy( &Queue->QueueStart[ i * Queue->ItemSize ], Ptr, Queue->ItemSize );
	Queue->ItemsAvailable++;

#if			defined QUEUE_STRICT_CHRONOLOGY
	if( Queue->ReadingOwner == NULL && ( p = Queue->TasksWaitingToRead ) != NULL )
		{
		Queue->ReadingOwner	= p;
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
	if(( p = Queue->TasksWaitingToRead ) != NULL )
		{
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
        /* ... remove the first task from the queue's event list. */
		RemoveTaskFromAllLists( p );
        /* Insert the task into the running tasks list. */
		if( InsertTaskInReadyTasksList( p ))
			{
			if( Queue->Mode & QUEUE_SWITCH_IN_ISR )
				return 2;
			}
		}

	return 1;
	}
/*============================================================================*/
int QueueItemsAvailable( queue_t *Queue )
	{
	if( Queue == NULL )
		return 0;

	return Queue->ItemsAvailable;
	}
/*============================================================================*/
int QueueFlush( queue_t *Queue, int Flag )
	{
	register intsave_t	s;
	context_t			*p;
	int					MustYield = 0;
	int					f = 0;

	if( Queue == NULL )
		return 0;

	s	= SaveAndDisableInterrupts();

	Queue->ItemsAvailable	= 0;
	Queue->ItemToRemove		= 0;
	Queue->ReadingOwner		= NULL;
	Queue->WritingOwner		= NULL;

	if( Flag & QUEUE_FLUSH_READING_TASKS )
		while(( p = Queue->TasksWaitingToRead ) != NULL )
			{
			f	|= QUEUE_FLUSH_READING_TASKS;
			RemoveTaskFromAllLists( p );
			InsertTaskInReadyTasksList( p );
	
			if(( Queue->Mode & QUEUE_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
	        	MustYield	= 1;
			}

	if( Flag & QUEUE_FLUSH_WRITING_TASKS )
		{
		while(( p = Queue->TasksWaitingToWrite ) != NULL )
			{
			f	|= QUEUE_FLUSH_WRITING_TASKS;
			RemoveTaskFromAllLists( p );
			InsertTaskInReadyTasksList( p );
	
			if(( Queue->Mode & QUEUE_SWITCH_IMMEDIATE ) && p->Priority >CurrentTask->Priority )
	        	MustYield	= 1;
			}
		}
	else if(( p = Queue->TasksWaitingToWrite ) != NULL )
		{
		Queue->WritingOwner	= p;
        /* ... remove the first task from the queue's event list. */
		RemoveTaskFromAllLists( p );
        /* Insert the task into the running tasks list. */
		InsertTaskInReadyTasksList( p );

		if(( Queue->Mode & QUEUE_SWITCH_IMMEDIATE ) && p->Priority >CurrentTask->Priority )
        	MustYield	= 1;
		}

	if( MustYield )
		ForceYield();

	RestoreInterrupts( s );

	return f;
	}
/*============================================================================*/
