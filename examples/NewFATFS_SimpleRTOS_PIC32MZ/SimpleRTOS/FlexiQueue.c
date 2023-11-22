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
#include "FlexiQueue.h"
/*============================================================================*/
int FlexiQueueInit( flexiqueue_t *Queue, unsigned int QueueLength, void *QueueBuffer, int Mode )
	{
	if( Queue == NULL )
		return 0;

#if			defined QUEUE_STRICT_CHRONOLOGY
	Queue->ReadingOwner			= NULL;
	Queue->WritingOwner			= NULL;
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
	Queue->TasksWaitingToWrite	= NULL;
	Queue->TasksWaitingToRead	= NULL;
	Queue->QueueLength			= QueueLength;
	Queue->BytesFree			= QueueLength;
	Queue->QueueBuffer			= (unsigned char*)QueueBuffer;
	Queue->ItemsAvailable		= 0;
	Queue->RemoveIndex			= 0;
	Queue->InsertIndex			= 0;
	Queue->Mode					= Mode;

	return 1;
	}
/*============================================================================*/
static inline __attribute((always_inline)) unsigned int EffectiveSize( unsigned int s )
	{
	return s + ( s > 128 ? 2 : 1 );
	}
/*============================================================================*/
static inline __attribute((always_inline)) unsigned int GetSizeOfNextItem( flexiqueue_t *q )
	{
	unsigned int	RemoveIndex, ItemLength;

	if( q->ItemsAvailable == 0 )
		return 0;

	RemoveIndex	= q->RemoveIndex;
	ItemLength	= (unsigned short)q->QueueBuffer[ RemoveIndex ];
	if( ++RemoveIndex >= q->QueueLength )
		RemoveIndex	= 0;
	if( ItemLength & 0x80 )
		{
		ItemLength	= ( ItemLength & 0x7f ) | ( (unsigned short)q->QueueBuffer[ RemoveIndex ] << 7 );
/*
		if( ++RemoveIndex >= q->QueueLength )
			RemoveIndex	= 0;
*/
		}
	return ItemLength + 1;
	}
/*============================================================================*/
#if         !defined min
#define min(a,b)    (( a ) < ( b ) ? ( a ) : ( b ))
#endif  /*  !defined min */

/*============================================================================*/
int FlexiQueueRead( flexiqueue_t *Queue, void *Ptr, unsigned int BufferSize, tickcount_t TimeToWait )
	{
	register intsave_t	s;
	context_t			*p;
	int					MustYield = 0;
	unsigned int		RemoveIndex, ItemLength, Aux, RemainingBytes;

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

		DeadLine	= SystemTick + TimeToWait;
		CurrentTask->ExtraParameter	= (void*)BufferSize;

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
		while(( (signed long)TimeToWait < 0 || (signed long)( DeadLine - SystemTick ) > 0 ) && Queue->ItemsAvailable == 0 );
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

	RemoveIndex	= Queue->RemoveIndex;
	ItemLength	= (unsigned short)Queue->QueueBuffer[ RemoveIndex ];
	if( ++RemoveIndex >= Queue->QueueLength )
		RemoveIndex	= 0;
	if( ItemLength > 127 )
		{
		ItemLength	= ( ItemLength & 0x7f ) | ( (unsigned short)Queue->QueueBuffer[ RemoveIndex ] << 7 );
		if( ++RemoveIndex >= Queue->QueueLength )
			RemoveIndex	= 0;
		}
	ItemLength++;

	if( BufferSize < ItemLength )
		{
		RestoreInterrupts( s );
		return -1;
		}

	RemainingBytes	= ItemLength;
	while(( Aux = min( RemainingBytes, Queue->QueueLength - RemoveIndex ) ) > 0 )
		{
		memcpy( (void*)Ptr, (void*)&Queue->QueueBuffer[ RemoveIndex ], Aux );
		Ptr	= (char*)Ptr + Aux;
		if(( RemoveIndex += Aux ) >= Queue->QueueLength )
			RemoveIndex	= 0;
		RemainingBytes	-= Aux;
		}

	Queue->RemoveIndex	= RemoveIndex;
	
	Queue->ItemsAvailable--;
	Queue->BytesFree	+= EffectiveSize( ItemLength );

#if			defined QUEUE_STRICT_CHRONOLOGY
	Queue->ReadingOwner		= NULL;
	/*------------------------------------------------------------------------*/
	/*
	 We got our item, now check to see whether there are more items and tasks
	 wanting them. This will set up a chain reaction.
	*/
	/*------------------------------------------------------------------------*/
	if( Queue->ItemsAvailable != 0 && ( p = Queue->TasksWaitingToRead ) != NULL )
		{
		for( Aux = GetSizeOfNextItem( Queue ); p != NULL && (unsigned int)p->ExtraParameter < Aux; p = p->Next )
			{}
		if( p != NULL )
			{
			Queue->ReadingOwner	= p;
			/* ... remove the first task from the queue's event list. */
			RemoveTaskFromAllLists( p );
			/* Insert the task into the running tasks list. */
			InsertTaskInReadyTasksList( p );

			if(( Queue->Mode & QUEUE_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
				MustYield	= 1;
			}
		}
	/*------------------------------------------------------------------------*/
	/*
	 We removed some bytes from the buffer, there should be room for more items
	 in the queue, check to see whether there is a task wanting to write and if
	 its item will fit in the buffer.
	*/
	/*------------------------------------------------------------------------*/
	if( Queue->WritingOwner == NULL && ( p = Queue->TasksWaitingToWrite ) != NULL
		&& EffectiveSize( (unsigned int)p->ExtraParameter ) <= Queue->BytesFree )
		{
		Queue->WritingOwner	= p;
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
	if(( p = Queue->TasksWaitingToWrite ) != NULL )
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

	return ItemLength;
	}
/*============================================================================*/
int FlexiQueueReadFromISR( flexiqueue_t *Queue, void *Ptr, unsigned int BufferSize )
	{
	context_t		*p;
	unsigned int	RemoveIndex, ItemLength, Aux, RemainingBytes;

	if( Queue == NULL )
		return 0;

#if			defined QUEUE_STRICT_CHRONOLOGY
	if( Queue->ItemsAvailable == 0 || Queue->ReadingOwner != NULL || Queue->TasksWaitingToRead != NULL )
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
	if( Queue->ItemsAvailable == 0 )
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
		return 0;

	RemoveIndex	= Queue->RemoveIndex;
	ItemLength	= (unsigned short)Queue->QueueBuffer[ RemoveIndex ];
	if( ++RemoveIndex >= Queue->QueueLength )
		RemoveIndex	= 0;
	if( ItemLength > 127 )
		{
		ItemLength	= ( ItemLength & 0x7f ) | ( (unsigned short)Queue->QueueBuffer[ RemoveIndex ] << 7 );
		if( ++RemoveIndex >= Queue->QueueLength )
			RemoveIndex	= 0;
		}
	ItemLength++;

	if( BufferSize < ItemLength )
		return -1;

	RemainingBytes	= ItemLength;
	while(( Aux = min( RemainingBytes, Queue->QueueLength - RemoveIndex ) ) > 0 )
		{
		memcpy( (void*)Ptr, (void*)&Queue->QueueBuffer[ RemoveIndex ], Aux );
		Ptr	= (char*)Ptr + Aux;
		if(( RemoveIndex += Aux ) >= Queue->QueueLength )
			RemoveIndex	= 0;
		RemainingBytes	-= Aux;
		}

	Queue->RemoveIndex	= RemoveIndex;

	Queue->ItemsAvailable--;
	Queue->BytesFree	+= EffectiveSize( ItemLength );

	/*------------------------------------------------------------------------*/
	/*
	 We removed some bytes from the buffer, there should be room for more items
	 in the queue, check to see whether there is a task wanting to write and if
	 its item will fit in the buffer.
	*/
	/*------------------------------------------------------------------------*/
#if			defined QUEUE_STRICT_CHRONOLOGY
	if( Queue->WritingOwner == NULL && ( p = Queue->TasksWaitingToWrite ) != NULL
		&& EffectiveSize( (unsigned int)p->ExtraParameter ) <= Queue->BytesFree )
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
				return ItemLength | 0x40000000;
			}
		}

	return ItemLength;
	}
/*============================================================================*/
int FlexiQueueWrite( flexiqueue_t *Queue, const void *Ptr, unsigned int ItemSize, tickcount_t TimeToWait )
	{
	register intsave_t	s;
	context_t			*p;
	unsigned int		InsertIndex, Aux, RemainingBytes;
	int					MustYield	= 0;

	if( Queue == NULL )
		return 0;

	if( EffectiveSize( ItemSize ) > Queue->QueueLength )
		return -1;

	s	= SaveAndDisableInterrupts();

#if			defined QUEUE_STRICT_CHRONOLOGY
	if( EffectiveSize( ItemSize ) > Queue->BytesFree || Queue->WritingOwner != NULL
		|| Queue->TasksWaitingToWrite != NULL )
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
	if( EffectiveSize( ItemSize ) > Queue->BytesFree )
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
		{
		tickcount_t	DeadLine;

		if( TimeToWait == 0 )
			{
			RestoreInterrupts( s );
			return 0;
			}

		DeadLine	= SystemTick + TimeToWait;
		CurrentTask->ExtraParameter	= (void*)ItemSize;

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
		while(( (signed long)TimeToWait < 0 || (signed long)( DeadLine - SystemTick ) > 0 ) && EffectiveSize( ItemSize ) > Queue->BytesFree );
#endif	/*	!defined QUEUE_STRICT_CHRONOLOGY */


#if			defined QUEUE_STRICT_CHRONOLOGY
		if( EffectiveSize( ItemSize ) > Queue->BytesFree || Queue->WritingOwner != CurrentTask )
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
		if( EffectiveSize( ItemSize ) > Queue->BytesFree )
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
			{
			RestoreInterrupts( s );
			return 0;
			}
		}

	Aux			= ItemSize - 1;
	InsertIndex	= Queue->InsertIndex;
	Queue->QueueBuffer[ InsertIndex ]	= ItemSize > 128 ? (unsigned char)( Aux | 0x80 ) : (unsigned char)( Aux & 0x7f );
	if( ++InsertIndex >= Queue->QueueLength )
		InsertIndex	= 0;
	if( ItemSize > 128 )
		{
		Queue->QueueBuffer[ InsertIndex ]	= (unsigned char)( Aux >> 7 );
		if( ++InsertIndex >= Queue->QueueLength )
			InsertIndex	= 0;
		}

	RemainingBytes	= ItemSize;
	while(( Aux = min( RemainingBytes, Queue->QueueLength - InsertIndex ) ) > 0 )
		{
		memcpy( (void*)&Queue->QueueBuffer[ InsertIndex ], (void*)Ptr, Aux );
		Ptr	= (char*)Ptr + Aux;
		if(( InsertIndex += Aux ) >= Queue->QueueLength )
			InsertIndex	= 0;
		RemainingBytes	-= Aux;
		}

	Queue->InsertIndex	= InsertIndex;
	
	Queue->ItemsAvailable++;
	Queue->BytesFree	-= EffectiveSize( ItemSize );

#if			defined QUEUE_STRICT_CHRONOLOGY
	Queue->WritingOwner		= NULL;

	if(( p = Queue->TasksWaitingToWrite ) != NULL && EffectiveSize( (unsigned int)p->ExtraParameter ) <= Queue->BytesFree )
		{
		Queue->WritingOwner	= p;
        /* ... remove the first task from the queue's event list. */
		RemoveTaskFromAllLists( p );
        /* Insert the task into the running tasks list. */
		InsertTaskInReadyTasksList( p );

		if(( Queue->Mode & QUEUE_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
			MustYield	= 1;
		}
	/*------------------------------------------------------------------------*/
	/*
	 We inserted some bytes into the buffer, let's check to see whether there is
	 a task wanting to read them.
	*/
	/*------------------------------------------------------------------------*/
	if( Queue->ReadingOwner == NULL && ( p = Queue->TasksWaitingToRead ) != NULL )
		{
		/*
		 Let's be practical, waking up a task that doesn't have room in its buffer
		 to receive the next item just to deny it access to the item is not wise,
		 let's try to find a task lower in the list that has room for the item.
		*/
		for( Aux = GetSizeOfNextItem( Queue ); p != NULL && (unsigned int)p->ExtraParameter < Aux; p = p->Next )
			{}
		if( p != NULL )
			{
			Queue->ReadingOwner	= p;
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
		if(( p = Queue->TasksWaitingToRead ) != NULL )
			@@openbrace@@
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
			/* ... remove the first task from the queue's event list. */
			RemoveTaskFromAllLists( p );
			/* Insert the task into the running tasks list. */
			InsertTaskInReadyTasksList( p );

			if(( Queue->Mode & QUEUE_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
				MustYield	= 1;
			}
		}

	if( MustYield )
		ForceYield();

	RestoreInterrupts( s );
	return 1;
	}
/*============================================================================*/
int FlexiQueueWriteFromISR( flexiqueue_t *Queue, const void *Ptr, unsigned int ItemSize )
	{
	context_t		*p;
	unsigned int	InsertIndex, Aux, RemainingBytes;

	if( Queue == NULL )
		return 0;

	if( EffectiveSize( ItemSize ) > Queue->QueueLength )
		return -1;

#if			defined QUEUE_STRICT_CHRONOLOGY
	if( EffectiveSize( ItemSize ) > Queue->BytesFree || Queue->WritingOwner != NULL || Queue->TasksWaitingToWrite != NULL )
#else	/*	defined QUEUE_STRICT_CHRONOLOGY */
	if( EffectiveSize( ItemSize ) > Queue->BytesFree )
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
		return 0;

	RemainingBytes	= ItemSize;
	InsertIndex		= Queue->InsertIndex;
	while(( Aux = min( RemainingBytes, Queue->QueueLength - InsertIndex ) ) > 0 )
		{
		memcpy( (void*)&Queue->QueueBuffer[ InsertIndex ], (void*)Ptr, Aux );
		Ptr	= (char*)Ptr + Aux;
		if(( InsertIndex += Aux ) >= Queue->QueueLength )
			InsertIndex	= 0;
		RemainingBytes	-= Aux;
		}

	Queue->InsertIndex	= InsertIndex;
	
	Queue->ItemsAvailable++;
	Queue->BytesFree	-= EffectiveSize( ItemSize );

#if			defined QUEUE_STRICT_CHRONOLOGY
	/*------------------------------------------------------------------------*/
	/*
	 We inserted some bytes into the buffer, let's check to see whether there is
	 a task wanting to read them.
	*/
	/*------------------------------------------------------------------------*/
	if( Queue->ReadingOwner == NULL && ( p = Queue->TasksWaitingToRead ) != NULL )
		{
		/*
		 Let's be practical, waking up a task that doesn't have room in its buffer
		 to receive the next item just to deny it access to the item is not wise,
		 let's try to find a task lower in the list that has room for the item.
		*/
		for( Aux = GetSizeOfNextItem( Queue ); p != NULL && (unsigned int)p->ExtraParameter < Aux; p = p->Next )
			{}
		if( p != NULL )
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
		}

	return 1;
	}
/*============================================================================*/
int FlexiQueueFlush( flexiqueue_t *Queue, int Flag )
	{
	register intsave_t	s;
	context_t			*p;
	int					MustYield	= 0;
	int					f = 0;

	if( Queue == NULL )
		return 0;

	s	= SaveAndDisableInterrupts();

	Queue->ItemsAvailable	= 0;
	Queue->RemoveIndex		= 0;
	Queue->InsertIndex		= 0;
	Queue->ReadingOwner		= NULL;
	Queue->WritingOwner		= NULL;
	Queue->BytesFree		= Queue->QueueLength;

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

			if(( Queue->Mode & QUEUE_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
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

		if(( Queue->Mode & QUEUE_SWITCH_IMMEDIATE ) && p->Priority > CurrentTask->Priority )
			MustYield	= 1;
		}

	if( MustYield )
		ForceYield();

	RestoreInterrupts( s );

	return f;
	}
/*============================================================================*/
