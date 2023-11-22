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
#if         !defined __PORTPIC18_H__
#define __PORTPIC18_H__
/*============================================================================*/
typedef unsigned long	tickcount_t;
/*============================================================================*/
typedef struct
	{
	unsigned char	*TopOfStack;

/*
	unsigned char	WREG;
	unsigned char	STATUS;
	unsigned char	FSR2L;
	unsigned char	FSR2H;
	unsigned char	FSR1L;
	unsigned char	FSR1H;
	unsigned char	FSR0L;
	unsigned char	FSR0H;
	unsigned char	INTCON;
	unsigned char	BSR;
	unsigned char	PRODL;
	unsigned char	PRODH;
	unsigned char	TABLAT;
	unsigned char	TBLPTRL;
	unsigned char	TBLPTRH;
	unsigned char	TBLPTRU;
	unsigned char	PCLATH;
	unsigned char	PCLATU;
	unsigned char	STKPTR;
	unsigned char	Stack[3*31];
*/
    /*------------------------------------------------------------------------*/

	unsigned int	errno;

    tickcount_t		TDelay;

	unsigned int	Priority;

	void			*PreviousDelayedTask;
	void			*NextDelayedTask;
	void			*DelayList;

    void			*Previous;
    void			*Next;
    void			*List;
    /*------------------------------------------------------------------------*/
    } context_t;
/*============================================================================*/
#define GetAddress(f)	(f)

void StartRTOS( tickcount_t TickPeriod );
/*============================================================================*/
#endif	/*	!defined __PORTPIC18_H__ */
/*============================================================================*/
