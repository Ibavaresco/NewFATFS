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
#include "SimpleRTOS.h"
/*============================================================================*/
/*
Points to the head of the unused contexts list. This list is organized as a 
single-linked linear list.
*/
/*============================================================================*/

int				RTOSStarted		= 0;

/*============================================================================*/
context_t		*FreeContexts	= NULL;

/*============================================================================*/
/*
Index of the highest priority ready tasks list that is not empty.
*/
/*============================================================================*/
unsigned int	HighestReadyPriority	= 0;

/*============================================================================*/
/*
Points to the head of the delayed tasks contexts list. This list is organized
as a double-linked linear list.
*/
/*============================================================================*/
context_t		*DelayedTasks	= NULL;

/*============================================================================*/
/*
Points to the currently running task. Sometimes it may not be in any list.
*/
/*============================================================================*/
context_t		*CurrentTask	= NULL;

/*============================================================================*/
/*
Tick counter, counts to a maximum and rolls-over to zero.
*/
/*============================================================================*/
tickcount_t		SystemTick;

/*============================================================================*/
/*
Signals that the running tasks contexts list was already changed by a vSleep.
*/
/*============================================================================*/
int				AlreadySwitched	= 0;

/*============================================================================*/
/*
Removes a task from any double-linked list.
*/
/*============================================================================*/
void RemoveTaskFromList( context_t *Task )
	{
	context_t	**List;
	context_t	*Previous, *Next;

	if( Task == NULL )
		return;

	/* Make 'List' point to the pointer to the head of the list. */
	List            = Task->List;

	if( List == NULL )
		return;

	/* Mark the task as not belonging to any list. */
	Task->List      = NULL;

	/* Make 'Previous' point to the task that precedes this one in the list (if any). */
	Previous        = Task->Previous;

	/* Un-link the task from the list.*/
	Task->Previous  = NULL;

	/* Make 'Next' point to the task that follows this one in the list (if any). */
	Next            = Task->Next;

	/* Un-link the task from the list. */
	Task->Next      = NULL;

	/* The task is the only one in a circular list... */
	if( Previous == Task )
		{
		/* ... just remove it from the list. */
		*List       = NULL;
		/* Finished */
		return;
		}

	/* The task is the head of the list */
	if( *List == Task )
		/* Make the next task (if any) be the head of the list. */
		*List   = Next;

	/* If there is a previous task... */
	if( Previous != NULL )
		/* ... make it point to the task that follows this one in the list (if any). */
		Previous->Next  = Next;

	/* If there is a next task... */
	if( Next != NULL )
		/* ... make it point to the task that precedes this one in the list (if any). */
		Next->Previous  = Previous;
	}
/*============================================================================*/
void RemoveTaskFromDelayList( context_t *Task )
	{
	context_t  *PreviousDelayedTask, *NextDelayedTask;

	if( Task == NULL )
		return;

	if( Task->DelayList == NULL )
		return;

	PreviousDelayedTask = Task->PreviousDelayedTask;
	NextDelayedTask     = Task->NextDelayedTask;

	/* If this task is the first one in the list of delayed tasks... */
	if( PreviousDelayedTask == NULL )
		/* ... make the next task in the list (if any) be the first. */
		DelayedTasks    = NextDelayedTask;
	else
		/* ... else make the task before this one in the list point to the task following this one (if any). */
		PreviousDelayedTask->NextDelayedTask	= NextDelayedTask;

	/* If there is a task after this one in the list... */
	if( NextDelayedTask != NULL )
		/* ... make it point to the task before this one in the list (if any). */
		NextDelayedTask->PreviousDelayedTask	= PreviousDelayedTask;

	/* Mark this task as not having a previous one in the delayed tasks list.*/
	Task->PreviousDelayedTask	= NULL;
	/* Mark this task as not having a next one in the delayed tasks list.*/
	Task->NextDelayedTask		= NULL;
	/* Mark this task as not belonging to the delayed tasks list.*/
	Task->DelayList				= NULL;
	}

/*============================================================================*/
void RemoveTaskFromAllLists( context_t *Task )
	{
	if( Task == NULL )
		return;

	if( Task->List != NULL )
		RemoveTaskFromList( Task );

	while( HighestReadyPriority > 0 && ReadyTasks[HighestReadyPriority] == NULL )
		HighestReadyPriority--;

	if( Task->DelayList != NULL )
		RemoveTaskFromDelayList( Task );
	}
/*============================================================================*/
static void InsertTaskInList( context_t *Task, context_t **List, int Mode )
	{
	context_t *First, *Last;

	if( Task == NULL )
		return;

	if( List == NULL )
		return;

	/* Mark the task as belonging to the list.*/
	Task->List	= List;

	/* The list is empty...*/
	if( *List == NULL )
		{
		/* ... insert the task into the list.*/
		*List			= Task;
		/* The list is circular, the task is the previous of itself.*/
		Task->Previous	= Task;
		/* If the list is circular, the task is the next to itself.*/
		Task->Next		= Mode == 0 ? NULL : Task;
		}
	else
		{
		/* Make 'First' point to the first element in the list.*/
		First			= *List;
		/* The last element of a circular list is the element before the first.*/
		Last			= First->Previous;
		/* We are inserting at the end, this element becomes the last one.*/
		First->Previous = Task;
		Task->Next		= Mode == 0 ? NULL : First;
		Last->Next		= Task;
		Task->Previous	= Last;
		}
	}
/*============================================================================*/
void InsertTaskInCircularList( context_t *Task, context_t **List )
    {
    InsertTaskInList( Task, List, 1 );
    }
/*============================================================================*/
void InsertTaskInLinearList( context_t *Task, context_t **List )
    {
    InsertTaskInList( Task, List, 0 );
    }
/*============================================================================*/
int InsertTaskInReadyTasksList( context_t *Task )
	{
	if( Task == NULL )
		return 0;

    /* Insert the task at the end of the running tasks list.*/
    InsertTaskInCircularList( Task, &ReadyTasks[Task->Priority] );

	if( Task->Priority > HighestReadyPriority )
		{
		HighestReadyPriority	= Task->Priority;
		return 1;
		}
	return 0;
	}
/*============================================================================*/
void InsertTaskInDelayList( context_t *Task, tickcount_t Time )
	{
	context_t		*p, *q, *Next;

	if( Task == NULL )
		return;

	/* Set the new task's time to wake.*/
	Task->TDelay	= Time;
	/* Make the new task belong to the delayed tasks list.*/
	Task->DelayList = &DelayedTasks;

	/* The list is empty...*/
	if( DelayedTasks == NULL )
		{
		/* ... just insert the task into the list.*/
		DelayedTasks                = Task;
		/* The list is linear, there is no previous task.*/
		Task->PreviousDelayedTask   = NULL;
		/* The list is linear, there is no next task.*/
		Task->NextDelayedTask       = NULL;
		}
	/* The list is not empty...*/
	else
		{
		tickcount_t	t;

		/* ... make 'p' point to the first element.*/
		p = DelayedTasks;
		/* Get the first element's time to wake.*/
		t = p->TDelay;
		/* The time to wake of the new task is less than the first element's...*/
		if( (signed long)( Time - t ) < 0 || ( (signed long)( Time - t ) == 0 && Task->Priority > p->Priority ))
			{
			/* ... insert the task as the first element.*/
			DelayedTasks                = Task;
			/* The task is now the first element, there is no previous one.*/
			Task->PreviousDelayedTask   = NULL;
			/* The former first element is now the next one of this task.*/
			Task->NextDelayedTask       = p;
			/* This task is now the previous one of the former first element.*/
			p->PreviousDelayedTask      = Task;
			}
		/* We need to find where the task is to be inserted...*/
		else
			{
			/* ... iterate through the remainig elements of the list (if any).*/
			for( q = p->NextDelayedTask; q != NULL; q = q->NextDelayedTask )
				{
				/* Get the time to wake of the task pointed to by 'q'.*/
				t = q->TDelay;
				/* The time to wake of the new task is less than the one pointed to by 'q'...*/
				if( (signed long)( Time - t ) < 0 || ( (signed long)( Time - t ) == 0 && Task->Priority > p->Priority ))
					/* ... stop.*/
					break;
				/* Advance one element.*/
				p = q;
				}
			/* We are inserting after the element pointed to by 'p'.*/
			Next                        = p->NextDelayedTask;
			Task->PreviousDelayedTask   = p;
			Task->NextDelayedTask       = Next;
			p->NextDelayedTask          = Task;
			/* There is a next element...*/
			if( Next != NULL )
				/* ... make it point to the new task.*/
				Next->PreviousDelayedTask   = Task;
			}
		}
	}
/*============================================================================*/
/* Test to see if it's time to wake the first delayed task.*/
void CheckDelayList( void )
    {
    context_t	*p;

    /* The delayed tasks list is not empty...*/
    while( ( p = DelayedTasks ) != NULL && (signed long)( p->TDelay - SystemTick ) <= 0 )
        {
        /* Remove the task from the delayed task list and from any event's list.*/
		RemoveTaskFromAllLists( p );
        /* Insert the task at the end of the running tasks list.*/
		InsertTaskInReadyTasksList( p );
        }
    }
/*============================================================================*/
void vSleep( tickcount_t t )
	{
	unsigned int		i, Aux;
	register intsave_t	s	= SaveAndDisableInterrupts();

	/* Count the priorities with ready tasks */
	for( i = 0, Aux = 0; i < MaxPriorities; i++ )
		if( ReadyTasks[i] != NULL )
			Aux++;

	/* The current task is the only running task, it shouldn't block */
	if( Aux < 2 && CurrentTask->Next == CurrentTask )
		{
		tickcount_t	t1, t2;

		t2	= t1	= GetTickCount();

		while( Aux < 2 && CurrentTask->Next == CurrentTask && t2 - t1 < t )
			{
			RestoreInterrupts( s );
			s	= SaveAndDisableInterrupts();

			t2	= GetTickCount();

			for( i = 0, Aux = 0; i < MaxPriorities; i++ )
				if( ReadyTasks[i] != NULL )
					Aux++;
			}

		if(( Aux >= 2 || CurrentTask->Next != CurrentTask ) && t2 - t1 < t )
			vSleep( t - ( t2 - t1 ));

		RestoreInterrupts( s );
		return;
		}

	/* Remove the task from the running tasks list.*/
	RemoveTaskFromAllLists( CurrentTask );

    /* Flag that the running tasks list alread changed.*/
    AlreadySwitched = 1;
    /* The sleep time is less than the maximum...*/
    if( (signed long)t >= 0 )
        /* Insert the task into the delayed tasks list.*/
        InsertTaskInDelayList( CurrentTask, SystemTick + t );
/*    // This may be useful in the future, but is pointless now.
    // The task is being suspended for undetermined time...
    else
        // ... insert the task into the suspended tasks list.
        InsertTaskInCircularList( CurrentTask, &SuspendedTasks );
*/
	ForceYield();
	RestoreInterrupts( s );
    }
/*============================================================================*/
int ChangeTaskPriority( context_t *Task, unsigned int NewPriority )
	{
	register intsave_t	s	= SaveAndDisableInterrupts();

	if( NewPriority >= MaxPriorities )
		NewPriority	= MaxPriorities - 1;

	if( Task == NULL )
		Task	= CurrentTask;

	if( Task->Priority == NewPriority )
		{
		RestoreInterrupts( s );
		return 1;
		}

	/* The task is in one of the ready tasks lists... */
	if( Task->List == &ReadyTasks[Task->Priority] )
		{
		RemoveTaskFromAllLists( Task );

		Task->Priority	= NewPriority;

		InsertTaskInReadyTasksList( Task );
		}
	/* The task is not in any of the ready tasks list... */
	else
		Task->Priority	= NewPriority;

	RestoreInterrupts( s );
	return 1;
	}
/*============================================================================*/
int ResumeTask( context_t *Task )
    {
	int					Result;
	register intsave_t	s	= SaveAndDisableInterrupts();

	/* The task is the current task or it is already in the ready tasks list... */
    if( Task == NULL || Task == CurrentTask || Task->List == &ReadyTasks[Task->Priority] )
		{
		RestoreInterrupts( s );
        return 0;
		}

	RemoveTaskFromAllLists( Task );

	/* Insert the task in the proper ready tasks list. */
	Result	= InsertTaskInReadyTasksList( Task );

	RestoreInterrupts( s );
    return Result;
    }
/*============================================================================*/
context_t *CreateTask( unsigned char *Stack, unsigned long StackSize, void *Parameter, void (*TaskFunc)( void* ), unsigned int Priority, void *FPUContext )
    {
	register intsave_t	s;
    context_t			*p;

	/* There is no free context to be used...*/
    if( FreeContexts == NULL )
    	{
        /* ... return failure.*/
        return NULL;
     	}   

	if( Priority >= MaxPriorities )
		Priority	= MaxPriorities - 1;

	s	= SaveAndDisableInterrupts();

	if( Priority > HighestReadyPriority )
		HighestReadyPriority	= Priority;

    /* Make 'p' point to the first free context.*/
    p   = FreeContexts;
    /* Remove the context from the free list.*/
    FreeContexts    = FreeContexts->Next;

    /* Initializes the context with the new task's address.*/
    ContextInit( p, Stack, StackSize, Parameter, TaskFunc, Priority, FPUContext );


    /* Insert the new task into the running tasks list.*/
	InsertTaskInReadyTasksList( p );

	RestoreInterrupts( s );
    /* Return the address of the new task's context.*/
    return p;
    }
/*============================================================================*/
int DeleteTask( context_t *Task )
	{
	register intsave_t	s	= SaveAndDisableInterrupts();

	if( Task == NULL )
		Task	= CurrentTask;

    /* The task is invalid or already deleted...*/
    if( Task->List == &FreeContexts )
		{
        /* ... finish.*/
		RestoreInterrupts( s );
        return 0;
		}

	RemoveTaskFromAllLists( Task );

    /* Link the task's context to the free contexts list*/
    Task->Next      = FreeContexts;
    FreeContexts    = Task;
    /* Mark the context as belonging to the free contexts list.*/
    Task->List      = &FreeContexts;

    /* The task is deleting itself...*/
    if( Task == CurrentTask )
		{
        ForceYield();

        /* ... return a status saying that the task MUST terminate.*/
        return 1;
		}
    /* The task is not deleting itself...*/
    else
		{
        /* ... return a status saying that the task should continue.*/
		RestoreInterrupts( s );
        return 0;
		}
    }
/*============================================================================*/
void TaskFinish( void )
	{
	DeleteTask( NULL );
	}
/*============================================================================*/
void Yield( void )
	{
	register intsave_t	s	= SaveAndDisableInterrupts();

	if( CurrentTask->Priority == HighestReadyPriority )
		ReadyTasks[HighestReadyPriority]
			= ReadyTasks[HighestReadyPriority]->Next;

	if( CurrentTask != ReadyTasks[HighestReadyPriority] )
		{
		AlreadySwitched	= 1;
        ForceYield();
		}
	else
		AlreadySwitched	= 0;

	RestoreInterrupts( s );
	}
/*============================================================================*/
tickcount_t GetTickCount( void )
	{
	register intsave_t	s;
	tickcount_t			t;
	
	s	= SaveAndDisableInterrupts();
	t	= SystemTick;
	RestoreInterrupts( s );

	return t;
	}
/*============================================================================*/
int RTOSIsRunning( void )
	{
	return RTOSStarted;
	}
/*============================================================================*/
