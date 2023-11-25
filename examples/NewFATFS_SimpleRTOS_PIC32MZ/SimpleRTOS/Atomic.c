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
#include "PortableInternals.h"
#include "Atomic.h"
/*============================================================================*/
void AtomicCopy( void *dst, const void *src, size_t size )
    {
    intsave_t   s   = SaveAndDisableInterrupts();
    memmove( dst, src, size );
    RestoreInterrupts( s );
    }
/*============================================================================*/
void AtomicCopyIRQ( void *dst, const void *src, size_t size, intsave_t (*IntCtrlFunc)( int Operation, intsave_t Argument ))
    {
    intsave_t   s   = SaveAndDisableInterrupts();
	intsave_t   i	= IntCtrlFunc( ATOMIC_SAVEANDDISABLE, 0 );
    memmove( dst, src, size );
	IntCtrlFunc( ATOMIC_RESTORE, i );
    RestoreInterrupts( s );
    }
/*============================================================================*/
void AtomicMemSet( void *dst, int val, size_t size )
	{
    intsave_t   s   = SaveAndDisableInterrupts();
    memset( dst, val, size );
    RestoreInterrupts( s );
	}
/*============================================================================*/
void AtomicMemSetIRQ( void *dst, int val, size_t size, intsave_t (*IntCtrlFunc)( int Operation, intsave_t Argument ))
	{
    intsave_t   s   = SaveAndDisableInterrupts();
	intsave_t   i	= IntCtrlFunc( ATOMIC_SAVEANDDISABLE, 0 );
    memset( dst, val, size );
	IntCtrlFunc( ATOMIC_RESTORE, i );
    RestoreInterrupts( s );
	}
/*============================================================================*/
