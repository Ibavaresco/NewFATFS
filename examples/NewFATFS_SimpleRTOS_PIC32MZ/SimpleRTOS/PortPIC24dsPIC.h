/*============================================================================*/
/*
SimpleRTOS - Very simple RTOS for Microcontrollers - PIC24/dsPIC port
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
#if         !defined __PORTPIC24DSPIC_H__
#define __PORTPIC24DSPIC_H__
/*============================================================================*/
typedef unsigned long	tickcount_t;
/*============================================================================*/
typedef struct
	{
	unsigned short	W0;
	unsigned short	W1;
	unsigned short	W2;
	unsigned short	W3;
	unsigned short	W4;
	unsigned short	W5;
	unsigned short	W6;
	unsigned short	W7;
	unsigned short	W8;
	unsigned short	W9;
	unsigned short	W10;
	unsigned short	W11;
	unsigned short	W12;
	unsigned short	W13;
	unsigned short	W14;
	unsigned short	W15;

	unsigned short	SR;

	unsigned short	W0Shadow;
	unsigned short	W1Shadow;
	unsigned short	W2Shadow;
	unsigned short	W3Shadow;

	unsigned short	SRShadow;

	unsigned short	SR_IPL3_PC;
	unsigned short	PC_SFA;

	unsigned short	SPLIM;
	unsigned short	TBLPAG;
	unsigned short	RCOUNT;
	unsigned short	CORCON;

	/*------------------------------------------------------------------------*/

#if			defined __PIC24FK__ || defined __PIC24F__ || defined __PIC24H__ || defined __dsPIC30F__ || defined __dsPIC33F__

	unsigned short	PSVPAG;
	unsigned short	Dummy;

#elif		defined __PIC24E__ || defined __dsPIC33E__

	unsigned short	DSRPAG;
	unsigned short	DSWPAG;

#else	/*	defined __PIC24E__ || defined __dsPIC33E__ */

	#error "Unknown processor!"

#endif	/*	defined __PIC24FK__ || defined __PIC24F__ || defined __PIC24H__ || defined __dsPIC30F__ || defined __dsPIC33F__ */

	/*------------------------------------------------------------------------*/
	/* Específicos dsPIC                                                      */
	/*------------------------------------------------------------------------*/

#if			defined __dsPIC30F__ || defined __dsPIC33F__ || defined __dsPIC33E__

	unsigned short	ACCAL;
	unsigned short	ACCAH;
	unsigned short	ACCAU;

	unsigned short	ACCBL;
	unsigned short	ACCBH;
	unsigned short	ACCBU;

	unsigned short	DCOUNT;
	unsigned short	DOENDL;
	unsigned short	DOENDH;
	unsigned short	DOSTARTL;
	unsigned short	DOSTARTH;
	unsigned short	DCOUNTShadow;
	unsigned short	DOENDLShadow;
	unsigned short	DOENDHShadow;
	unsigned short	DOSTARTLShadow;
	unsigned short	DOSTARTHShadow;

#endif	/*	defined __dsPIC30F__ || defined __dsPIC33F__ || defined __dsPIC33E__ */

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

    void            *ExtraParameter;
    /*------------------------------------------------------------------------*/
    } context_t;
/*============================================================================*/
#define GetAddress(f)	(f)

void __attribute__((noreturn)) StartRTOS( tickcount_t TickPeriod );
/*============================================================================*/
#endif	/*	!defined __PORTPIC24DSPIC_H__ */
/*============================================================================*/
