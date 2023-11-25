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
#if			!defined __FLEXIQUEUE_H__
#define __FLEXIQUEUE_H__
/*============================================================================*/
#include "SimpleRTOS.h"
/*============================================================================*/

/* In this mode, a higher priority task awaken will only run in the next tick */
#define	QUEUE_SWITCH_NORMAL		0

/* In this mode, a higher priority task will run immediately when awaken from another task */
#define	QUEUE_SWITCH_IMMEDIATE	1

/* In this mode, a higher priority task will run immediately when awaken from an ISR */
#define	QUEUE_SWITCH_IN_ISR		2

/*============================================================================*/

#define	QUEUE_FLUSH_DATA_ONLY		0
#define	QUEUE_FLUSH_READING_TASKS	1
#define	QUEUE_FLUSH_WRITING_TASKS	2
#define	QUEUE_FLUSH_ALL				3

/*============================================================================*/

typedef struct
	{
#if			defined QUEUE_STRICT_CHRONOLOGY
	context_t		*WritingOwner;
	context_t		*ReadingOwner;
#endif	/*	defined QUEUE_STRICT_CHRONOLOGY */
	context_t		*TasksWaitingToWrite;
	context_t		*TasksWaitingToRead;
	unsigned int	QueueLength;
	unsigned char	*QueueBuffer;
	unsigned int	BytesFree;
	unsigned int	ItemsAvailable;
	unsigned int	RemoveIndex;
	unsigned int	InsertIndex;
	int				Mode;
	} flexiqueue_t;

/*============================================================================*/

int		FlexiQueueInit			( flexiqueue_t *Queue, unsigned int QueueLength, void *QueueBuffer, int Mode );
int		FlexiQueueRead			( flexiqueue_t *Queue, void *Ptr, unsigned int BufferSize, tickcount_t TimeToWait );
int		FlexiQueueReadFromISR	( flexiqueue_t *Queue, void *Ptr, unsigned int BufferSize );
int		FlexiQueueWrite			( flexiqueue_t *Queue, const void *Ptr, unsigned int ItemSize, tickcount_t TimeToWait );
int		FlexiQueueWriteFromISR	( flexiqueue_t *Queue, const void *Ptr, unsigned int ItemSize );
int		FlexiQueueFlush			( flexiqueue_t *Queue, int Flag );

/*============================================================================*/
#endif	/*	!defined __FLEXIQUEUE_H__ */
/*============================================================================*/
