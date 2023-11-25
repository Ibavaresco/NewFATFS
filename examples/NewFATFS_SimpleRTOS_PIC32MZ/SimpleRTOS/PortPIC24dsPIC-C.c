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
#if			defined __XC16__
/*============================================================================*/
#include <string.h>
#include "PortPIC24dsPIC.h"
#include "SimpleRTOSInternals.h"
/*============================================================================*/
void TickHook( void );
/*============================================================================*/
static void Switch( void )
	{
	IFS0bits.T1IF	= 0; 	/* clear the interrupt flag */

	SystemTick++;

	CheckDelayList();

	/* The current task ran for at least one full time slice... */
	if( AlreadySwitched == 0 && CurrentTask->Priority == HighestReadyPriority )
		/* ... it doesn't deserve an additional time slice. */
		ReadyTasks[HighestReadyPriority]
			= ReadyTasks[HighestReadyPriority]->Next;

	CurrentTask			= ReadyTasks[HighestReadyPriority];

	/*
	The upcoming task will run from the very beginning of its time slice,
	at the end of this slice it will be switched off.
	*/
	AlreadySwitched		= 0;

    if( TickHook != NULL )
        TickHook();
	}
/*============================================================================*/
void ContextInit( context_t *Context, unsigned char *Stack, unsigned long StackSize, void *Parameter, void (*TaskFunc)( void* ), unsigned int Priority )
    {
	unsigned long	FuncAddress;

	memset( Context, 0x00, sizeof( context_t ));

	FuncAddress			= (unsigned short)TaskFunc;

	Context->W0			= (unsigned short)Parameter;
	Context->W15		= (unsigned short)Stack;

	Context->SR			= 0x0000;

	/*
	The IPL bits must be zero for the interrupts to be enabled when the
	task starts.
	*/
	Context->SR_IPL3_PC	= (( 0 /*SR*/	 <<  8 ) & 0xff00 ) |
						  (( CORCON		 <<  4 ) & 0x0080 ) |
						  (( FuncAddress >> 16 ) & 0x007f );

	/*
	The SFA bit must be zero on function entry because a call instrustion always
	clears it.
	*/
	Context->PC_SFA					= ( FuncAddress & 0xfffe ) | (( 0 /*CORCON*/ >> 2 ) & 0x0001 );

	Context->SPLIM					= (unsigned short)Stack + StackSize - 2;
	Context->TBLPAG					= TBLPAG;

	/*
	The RCOUNT register must be zero on function entry.
	*/
	Context->RCOUNT					= 0;
	Context->CORCON					= CORCON;


#if			defined __PIC24H__ || defined __PIC24F__ || defined __PIC24FK__

	Context->PSVPAG					= PSVPAG;

#elif		defined __PIC24E__

	Context->DSRPAG					= DSRPAG;
	Context->DSWPAG					= DSWPAG;

#endif	/*	defined __PIC24H__ || defined __PIC24F__ || defined __PIC24FK__ */

	Context->Priority				= Priority;
    Context->TDelay					= 0;
	Context->PreviousDelayedTask	= 0;
	Context->NextDelayedTask		= 0;
	Context->DelayList				= 0;

    #ifdef      USE_SLICE_LENGTH
        Context->SliceLength		= 0;
    #endif  /*  USE_SLICE_LENGTH */

    Context->Previous				= 0;
    Context->Next					= 0;
    Context->List					= 0;
    }
/*============================================================================*/
void InitRTOS( context_t *Contexts, unsigned int Number )
	{
    unsigned int i;

	/* No contexts to be initialized... The RTOS is useless.*/
    if( Contexts == NULL || Number == 0 )
	    {
        FreeContexts        = NULL;
        return;
        }

	CurrentTask		= Contexts;

	for( i = 0; i < MaxPriorities; i++ )
		ReadyTasks[i]	= NULL;

	/*------------------------------------------------------------------------*/
	/* Initialize all contexts and link them to the free contexts list.*/
    FreeContexts		= Contexts;
    for( i = 0; i < Number - 1; i++, Contexts++ )
        {
        Contexts->Next	= Contexts + 1;
        Contexts->List	= &FreeContexts;
        }
    Contexts->Next		= NULL;
    Contexts->List		= &FreeContexts;

	/*------------------------------------------------------------------------*/
	}
/*============================================================================*/
void __attribute__((naked)) SaveContext( void )
    {
    /*------------------------------------------------------------------------*/
	asm volatile(
		"push.w		W4					\n"
		"mov.w		_CurrentTask,W4		\n"
		"mov.w		W0,[W4+0]			\n"
		"pop.w		W0					\n"
		"mov.w		W0,[W4+8]			\n"

		"mov.w		W1,[W4+2]			\n"
		"mov.w		W2,[W4+4]			\n"
		"mov.w		W3,[W4+6]			\n"

		"mov.w		W5,[W4+10]			\n"
		"mov.w		W6,[W4+12]			\n"
		"mov.w		W7,[W4+14]			\n"
		"mov.w		W8,[W4+16]			\n"
		"mov.w		W9,[W4+18]			\n"
		"mov.w		W10,[W4+20]			\n"
		"mov.w		W11,[W4+22]			\n"
		"mov.w		W12,[W4+24]			\n"
		"mov.w		W13,[W4+26]			\n"
		"mov.w		W14,[W4+28]			\n"

		"mov.w		SR,W0				\n"
		"mov.w		W0,[W4+32]			\n"
		);

	asm volatile(
		"pop.s							\n"
		"mov.w		W0,[W4+34]			\n"
		"mov.w		W1,[W4+36]			\n"
		"mov.w		W2,[W4+38]			\n"
		"mov.w		W3,[W4+40]			\n"
		"mov.w		SR,W0				\n"
		"mov.w		W0,[W4+42]			\n"
		);

	asm volatile(
		"mov.w		[W15-6],W0			\n"
		"mov.w		W0,[W4+44]			\n"
		"mov.w		[W15-8],W0			\n"
		"mov.w		W0,[W4+46]			\n"

		"sub.w		W15,#8,W0			\n"
		"mov.w		W0,[W4+30]			\n"

		"mov.w		SPLIM,W0			\n"
		"mov.w		W0,[W4+48]			\n"
		"mov.w		TBLPAG,W0			\n"
		"mov.w		W0,[W4+50]			\n"
		"mov.w		RCOUNT,W0			\n"
		"mov.w		W0,[W4+52]			\n"
		"mov.w		CORCON,W0			\n"
		"mov.w		W0,[W4+54]			\n"
		);

		/*--------------------------------------------------------------------*/

#if			defined __dsPIC30F__ || defined __dsPIC33F__
	asm volatile(
		"mov.w		PSVPAG,W0			\n"
		"mov.w		W0,[W4+56]			\n"
		);
#elif		defined __dsPIC33E__
	asm volatile(
		"mov.w		DSRPAG,W0			\n"
		"mov.w		W0,[W4+56]			\n"
		"mov.w		DSWPAG,W0			\n"
		"mov.w		W0,[W4+58]			\n"
		);
#endif	/*	defined __dsPIC30F__ || defined __dsPIC33F__ */

		/*--------------------------------------------------------------------*/
		/* Específicos dsPIC                                                  */
		/*--------------------------------------------------------------------*/

#if			defined __dsPIC30F__ || defined __dsPIC33F__ || defined __dsPIC33E__

	asm volatile(

		"mov.w		ACCAL,W0			\n"
		"mov.w		W0,[W4+60]			\n"
		"mov.w		ACCAH,W0			\n"
		"mov.w		W0,[W4+62]			\n"
		"mov.w		ACCAU,W0			\n"
		"mov.w		W0,[W4+64]			\n"
		"mov.w		ACCBL,W0			\n"
		"mov.w		W0,[W4+66]			\n"
		"mov.w		ACCBH,W0			\n"
		"mov.w		W0,[W4+68]			\n"
		"mov.w		ACCBU,W0			\n"
		"mov.w		W0,[W4+70]			\n"
		);

	asm volatile(
	
	/* Save DO vars */
		"mov.w		DCOUNT,W0			\n"
		"mov.w		W0,[W4+72]			\n"
		"mov.w		DOENDL,W0			\n"
		"mov.w		W0,[W4+74]			\n"
		"mov.w		DOENDH,W0			\n"
		"mov.w		W0,[W4+76]			\n"
		"mov.w		DOSTARTL,W0			\n"
		"mov.w		W0,[W4+78]			\n"
		"mov.w		DOSTARTH,W0			\n"
		"mov.w		W0,[W4+80]			\n"

	/* DL > 0 ? */
		"mov.b		[W4+55],W2			\n"
		"and.w		#7,W2				\n"
		"bra		z,JumpX				\n"
		);

	asm volatile(

	/* Abort DO */
		"mov.w		#tbloffset(Label1),W0	\n"
		"mov.w		W0,DOSTARTL				\n"
		"mov.w		#tblpage(Label1),W0		\n"
		"mov.w		W0,DOSTARTH				\n"
		"mov.w		#tbloffset(Label2),W0	\n"
		"mov.w		W0,DOENDL				\n"
		"mov.w		#tblpage(Label2),W0		\n"
		"mov.w		W0,DOENDH				\n"

		"mov.w		#1,W2					\n"

"Label1:									\n"
		"mov.w		W2,DCOUNT				\n"
		"bset.w		CORCON,#11				\n"
"Label2: nop								\n"

	/* DL > 0 ? */
		"btss.w		SR,#9					\n"
		"bra		JumpX					\n"
		);

	/* Save DO shadows */
	asm volatile(
		"mov.w		DCOUNT,W0			\n"
		"mov.w		W0,[W4+82]			\n"
		"mov.w		DOENDL,W0			\n"
		"mov.w		W0,[W4+84]			\n"
		"mov.w		DOENDH,W0			\n"
		"mov.w		W0,[W4+86]			\n"
		"mov.w		DOSTARTL,W0			\n"
		"mov.w		W0,[W4+88]			\n"
		"mov.w		DOSTARTH,W0			\n"
		"mov.w		W0,[W4+90]			\n"

	/* Abort DO */
		"bset.w		CORCON,#11			\n"
"JumpX:									\n"
		);

#endif	/*	defined __dsPIC30F__ || defined __dsPIC33F__ || defined __dsPIC33E__ */
		/*--------------------------------------------------------------------*/
	}
/*============================================================================*/
void __attribute__((naked)) RestoreContext( void )
	{
	asm volatile(
		"mov.w		_CurrentTask,W4		\n"
		);

		/*--------------------------------------------------------------------*/
		/* Específicos dsPIC                                                  */
		/*--------------------------------------------------------------------*/

#if			defined __dsPIC30F__ || defined __dsPIC33F__ || defined __dsPIC33E__

	asm volatile(

	/* DL > 0 ? */
		"mov.b		[W4+45],W2			\n"
		"and.w		#7,W2				\n"
		"bra		z,JumpA				\n"

	/* DL > 1 ? */
		"dec.w		W2,W0				\n"
		"bra		z,JumpB				\n"

	/* Start DO */
		"do			#1,Label0			\n"

	/* DL = DL - 1 */
		"dec.w		W2,W2				\n"
		);

	/* Restore DO shadows */
	asm volatile(
		"mov.w		[W4+90],W0			\n"
		"mov.w		W0,DOSTARTH			\n"
		"mov.w		[W4+88],W0			\n"
		"mov.w		W0,DOSTARTL			\n"
		"mov.w		[W4+86],W0			\n"
		"mov.w		W0,DOENDH			\n"
		"mov.w		[W4+84],W0			\n"
		"mov.w		W0,DOENDL			\n"
		"mov.w		[W4+82],W0			\n"
		"mov.w		W0,DCOUNT			\n"
		);

	asm volatile(

"JumpB:									\n"

	/* Start DO */
		"do			#1,Label0			\n"

	/* DL = DL - 1 */
		"dec.w		W2,W2				\n"

	/* DL > 0 ? */
		"bra		nz,JumpB			\n"

"JumpA:									\n"
	/* Restore DO vars */

		"mov.w		[W4+80],W0			\n"
		"mov.w		W0,DOSTARTH			\n"
		"mov.w		[W4+78],W0			\n"
		"mov.w		W0,DOSTARTL			\n"
		"mov.w		[W4+76],W0			\n"
		"mov.w		W0,DOENDH			\n"
		"mov.w		[W4+74],W0			\n"
		"mov.w		W0,DOENDL			\n"
		"mov.w		[W4+72],W0			\n"
"Label0: mov.w		W0,DCOUNT			\n"
		);

	/* Restore DSP accumulators */
	asm volatile(
		"mov.w		[W4+70],W0			\n"
		"mov.w		W0,ACCBU			\n"
		"mov.w		[W4+68],W0			\n"
		"mov.w		W0,ACCBH			\n"
		"mov.w		[W4+66],W0			\n"
		"mov.w		W0,ACCBL			\n"
		"mov.w		[W4+64],W0			\n"
		"mov.w		W0,ACCAU			\n"
		"mov.w		[W4+62],W0			\n"
		"mov.w		W0,ACCAH			\n"
		"mov.w		[W4+60],W0			\n"
		"mov.w		W0,ACCAL			\n"
		);

#endif	/*	defined __dsPIC30F__ || defined __dsPIC33F__ || defined __dsPIC33E__ */

		/*--------------------------------------------------------------------*/

		/* Restore program space visibility registers */

#if			defined __dsPIC30F__ || defined __dsPIC33F__

	asm volatile(
		"mov.w		[W4+56],W0			\n"
		"mov.w		W0,PSVPAG			\n"
		);

#elif		defined __dsPIC33E__

	asm volatile(
		"mov.w		[W4+58],W0			\n"
		"mov.w		W0,DSWPAG			\n"
		"mov.w		[W4+56],W0			\n"
		"mov.w		W0,DSRPAG			\n"
		);

#endif	/*	defined __dsPIC30F__ || defined __dsPIC33F__ */

		/*--------------------------------------------------------------------*/

	/* Restore core control registers */
	asm volatile(
		"mov.w		[W4+54],W0			\n"
		"mov.w		W0,CORCON			\n"
		"mov.w		[W4+52],W0			\n"
		"mov.w		W0,RCOUNT			\n"
		"mov.w		[W4+50],W0			\n"
		"mov.w		W0,TBLPAG			\n"
		"mov.w		[W4+48],W0			\n"
		"mov.w		W0,SPLIM			\n"

		"mov.w		[W4+30],W15			\n"

		"mov.w		[W4+46],W0			\n"
		"push.w		W0					\n"
		"mov.w		[W4+44],W0			\n"
		"push.w		W0					\n"
		);

	/* Restore shadow registers */
	asm volatile(
		"mov.w		[W4+42],W0			\n"
		"mov.w		W0,SR				\n"
		"mov.w		[W4+40],W3			\n"
		"mov.w		[W4+38],W2			\n"
		"mov.w		[W4+36],W1			\n"
		"mov.w		[W4+34],W0			\n"
		"push.s							\n"
		);

	/* Restore main registers */
	asm volatile(
		"mov.b		[W4+33],W0			\n"
		"mov.b		WREG,0x0043			\n"

		"mov.w		[W4+28],W14			\n"
		"mov.w		[W4+26],W13			\n"
		"mov.w		[W4+24],W12			\n"
		"mov.w		[W4+22],W11			\n"
		"mov.w		[W4+20],W10			\n"
		"mov.w		[W4+18],W9			\n"
		"mov.w		[W4+16],W8			\n"
		"mov.w		[W4+14],W7			\n"
		"mov.w		[W4+12],W6			\n"
		"mov.w		[W4+10],W5			\n"

		"mov.w		[W4+6],W3			\n"
		"mov.w		[W4+4],W2			\n"
		"mov.w		[W4+2],W1			\n"

		"mov.w		[W4+0],W0			\n"
		"mov.w		[W4+8],W4			\n"

		"retfie							\n"
		);
	}
/*============================================================================*/
void __attribute__((noreturn)) StartRTOS( tickcount_t TickPeriod )
	{
    SystemTick		= 0;
	CurrentTask		= ReadyTasks[HighestReadyPriority];

	PR1				= TickPeriod;
	TMR1			= 0;
	T1CON			= 0x8000;

	IFS0bits.T1IF	= 0;
	IEC0bits.T1IE	= 1;
	IPC0bits.T1IP	= 1;

	SR			   |= 0x00e0;

	INTCON1			= 0x0000;
	INTCON2			= 0x8000;

	RestoreContext();

	while( 1 )
		{}
    }
/*============================================================================*/
void __attribute__((interrupt,naked,auto_psv)) _T1Interrupt( void )
    {
    /*------------------------------------------------------------------------*/
	SaveContext();

	Switch();

	/*
	Doing it this way to save stack and also because some dsPICs and PIC24 execute
	'bra' and 'goto' faster than 'call' or 'rcall'. We are not returnig from here
	anyway.
	*/
	asm volatile(
		"bra		_RestoreContext	\n"
		 );
    /*------------------------------------------------------------------------*/
	}
/*============================================================================*/
#if			defined __DEBUG

void T2Handler( void );

void __attribute__((interrupt,naked,auto_psv)) _T2Interrupt( void )
    {
    /*------------------------------------------------------------------------*/
	SaveContext();

	T2Handler();

	/*
	Doing it this way to save stack and also because some dsPICs and PIC24 execute
	'bra' and 'goto' faster than 'call' or 'rcall'. We are not returnig from here
	anyway.
	*/
	asm volatile(
		"bra		_RestoreContext	\n"
		 );
    /*------------------------------------------------------------------------*/
	}

#endif	/*	defined __DEBUG */
/*============================================================================*/
void __attribute__((naked)) ForceYield( void )
	{
    /*------------------------------------------------------------------------*/
	/*
	Transform the return address to a form suitable for
	a 'retfie' instruction.
	*/
	asm volatile(
		"mov.b		SR,WREG				\n"
		"mov.b		W0,[W15-1]			\n"
		);

	SaveContext();

	CurrentTask	= ReadyTasks[HighestReadyPriority];

	/*
	Doing it this way to save stack and also because some dsPICs and PIC24 execute
	'bra' and 'goto' faster than 'call' or 'rcall'. We are not returnig from here
	anyway.
	*/
	asm volatile(
		"bra		_RestoreContext	\n"
		 );
    /*------------------------------------------------------------------------*/
	}
/*============================================================================*/
void __attribute__((naked)) HigherPriorityAwakened( void )
	{
	CurrentTask	= ReadyTasks[HighestReadyPriority];

	/*
	Doing it this way to save stack and also because some dsPICs and PIC24 execute
	'bra' and 'goto' faster than 'call' or 'rcall'. We are not returnig from here
	anyway.
	*/
	asm volatile(
		"bra		_RestoreContext	\n"
		 );
	}
/*============================================================================*/
#endif	/*	defined __XC16__ */
/*============================================================================*/
