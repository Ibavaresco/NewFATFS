/*============================================================================*/
/*
SimpleRTOS - Very simple RTOS for Microcontrollers - PIC18 port
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
#if         !defined __PORTPIC18INTERNALS_H__
#define __PORTPIC18INTERNALS_H__
/*============================================================================*/
#include "SimpleRTOS.h"
/*============================================================================*/
typedef unsigned char	intsave_t;
/*============================================================================*/

extern intsave_t	SaveAndDisableInterrupts( void );
extern void			RestoreInterrupts		( intsave_t Status );

/*============================================================================*/
unsigned char ram *	GetMATHDATAStrt			( void );
unsigned char		GetMATHDATALen			( void );
unsigned char ram *	GetTMPDATAStrt			( void );
unsigned char		GetTMPDATALen			( void );
void				SaveTMP_MATH			( void );
void				RestoreTMP_MATH			( void );
/*============================================================================*/
void				HigherPriorityAwakened	( void );
void				ForceYield				( void );
/*============================================================================*/
#endif	/*	!defined __PORTPIC18INTERNALS_H__ */
/*============================================================================*/
