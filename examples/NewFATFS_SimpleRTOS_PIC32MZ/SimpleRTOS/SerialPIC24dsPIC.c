/*============================================================================*/
/*
    FreeRTOS V7.6.0 - Copyright (C) 2013 Real Time Engineers Ltd. 
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/
/*============================================================================*/

/*============================================================================*/
/* BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER. 

NOTE:  This driver is primarily to test the scheduler functionality.  It does
not effectively use the buffers or DMA and is therefore not intended to be
an example of an efficient driver. */
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*

Note: This file was severely modified from the original FreeRTOS version.

*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
#if			defined __XC16__
/*============================================================================*/
/* Standard include file. */
#include <stdlib.h>

/*============================================================================*/

/* Scheduler include files. */
#include "Portable.h"

#include "Queue.h"
#include "PortPIC24dsPICInternals.h"

/*============================================================================*/

/* Demo app include files. */
#include "SerialPIC24dsPIC.h"

/*============================================================================*/

/* The queues used to communicate between tasks and ISR's. */
#define QUEUE_LENGTH		32
/*static*/ queue_t			QueuesRX[2]; 
/*static*/ unsigned char	BuffersRX[2][QUEUE_LENGTH]; 
/*static*/ queue_t			QueuesTX[2]; 
/*static*/ unsigned char	BuffersTX[2][QUEUE_LENGTH]; 

/* Flag used to indicate the tx status. */

#define	configPERIPHERAL_CLOCK_HZ	40000000

/*============================================================================*/
int SerialPortInitMinimal( unsigned int port, unsigned long BaudRate )
	{
	/*unsigned short	usBRG;*/

	int				index;

	if( port == 1 )
		index	= 0;
	else if( port == 2 )
		index	= 1;
	else
		return 0;

	/* Create the queues used by the com test task. */
	QueueInit( &QueuesRX[index], sizeof( signed char ), QUEUE_LENGTH, &BuffersRX, QUEUE_SWITCH_IN_ISR );
	QueueInit( &QueuesTX[index], sizeof( signed char ), QUEUE_LENGTH, &BuffersTX, QUEUE_NORMAL );

#if 0
	/* Configure the UART and interrupts. */
	usBRG				= (unsigned short)(( (float)configPERIPHERAL_CLOCK_HZ / ( (float)16 * (float)BaudRate ) ) - (float)0.5);

	if( port == 1 )	
		{
		PORTSetPinsDigitalIn( IOPORT_B, BIT_2 );
		PORTSetPinsDigitalOut( IOPORT_B, BIT_3 );
		U1RXRbits.U1RXR		= 4;
		RPB3Rbits.RPB3R		= 1;

		OpenUART1( UART_EN, UART_RX_ENABLE | UART_TX_ENABLE | UART_INT_TX | UART_INT_RX_CHAR, usBRG );

		INTClearFlag( INT_U1RX );
		INTClearFlag( INT_U1TX );
		INTSetVectorPriority( INT_UART_1_VECTOR, configKERNEL_INTERRUPT_PRIORITY + 1 );
		INTSetVectorSubPriority( INT_UART_1_VECTOR, UART_INT_SUB_PR0 );
		INTEnable( INT_U1RX, INT_ENABLED );		
		INTEnable( INT_U1TX, INT_DISABLED );
		}
	else
		{
		PORTSetPinsDigitalIn( IOPORT_B, BIT_8 );
		PORTSetPinsDigitalOut( IOPORT_B, BIT_9 );
		U2RXRbits.U2RXR		= 4;
		RPB9Rbits.RPB9R		= 2;

		OpenUART2( UART_EN, UART_RX_ENABLE | UART_TX_ENABLE | UART_INT_TX | UART_INT_RX_CHAR, usBRG );

		INTClearFlag( INT_U2RX );
		INTClearFlag( INT_U2TX );
		INTSetVectorPriority( INT_UART_2_VECTOR, configKERNEL_INTERRUPT_PRIORITY + 1 );
		INTSetVectorSubPriority( INT_UART_2_VECTOR, UART_INT_SUB_PR0 );
		INTEnable( INT_U2RX, INT_ENABLED );
		INTEnable( INT_U2TX, INT_DISABLED );
		}	
#endif
	return 1;
	}

/*============================================================================*/

int SerialReceive( unsigned int port, tickcount_t TimeToWait )
	{
	unsigned char	c;
	int				index;

	if( port != 1 && port != 2 )
		return -2;
	index	= port == 2 ? 1 : 0;
	
	if( QueueRead( &QueuesRX[index], &c, TimeToWait ))
		return (unsigned int)c;
	else
		return -1;
	}

/*============================================================================*/

int SerialWaitForTXCompletion( unsigned int port, tickcount_t TimeToWait )
	{
	return 0;
	}

/*============================================================================*/

int SerialTransmit( unsigned int port, signed char c, tickcount_t TimeToWait )
	{
	int	index;

	if( port != 1 && port != 2 )
		return -2;
	index	= port == 2 ? 1 : 0;

	/* Return false if after the block time there is no room on the Tx queue. */
	if( QueueWrite( &QueuesTX[index], &c, TimeToWait ) == 0 )
		return 0;
	else
		{
/*
		if( index )
			IEC1bits.U2TXIE	= 1;
		else
*/
			IEC0bits.U1TXIE	= 1;

		return 1;
		}
	return 0;
	}

/*============================================================================*/

void U1Handler( void )
	{
	int			MustSwitch	= 0;
	/* Declared static to minimise stack use. */
	static char	cChar;
	
	/* Are any Rx interrupts pending? */
	if( IFS0bits.U1RXIF == 1)
		{
		while( U1STAbits.URXDA )
			{
			/* Retrieve the received character and place it in the queue of
			received characters. */
			cChar = U1RXREG;
			if( QueueWriteFromISR( &QueuesRX[0], &cChar ) > 1 )
				MustSwitch	= 1;
			}
		IFS0bits.U1RXIF	= 0;
		}
	
	/* Are any Tx interrupts pending? */
	if( IFS0bits.U1TXIF == 1 )
		{
		while( ( U1STAbits.UTXBF ) == 0 )
			{
			int	Result;

			Result = QueueReadFromISR( &QueuesTX[0], &cChar );
			if( Result > 0 )
				{
				/* Send the next character queued for Tx. */
				U1TXREG = cChar;
				if( Result > 1 )
					MustSwitch	= 1;
				}
			else
				{
				/* Queue empty, nothing to send. */
				IFS0bits.U1TXIF	= 0;
				IEC0bits.U1TXIE	= 0;
				break;
				}
			}
		IFS0bits.U1TXIF	= 0;
		}

	if( MustSwitch )
    	HigherPriorityAwakened();
	}
/*============================================================================*/
#if 0
#if			defined __DEBUG

void U2Handler( void )
	{
	int			MustSwitch	= 0;
	int			Result;
	/* Declared static to minimise stack use. */
	static char	cChar;
	
	/* Are any Rx interrupts pending? */
	if( IFS1bits.U2RXIF == 1 )
		{
		while( U2STAbits.URXDA )
			{
			/* Retrieve the received character and place it in the queue of
			received characters. */
			cChar = U2RXREG;
			if( QueueWriteFromISR( &QueuesRX[0], &cChar ) > 1 )
				MustSwitch	= 1;
			}
		IFS1bits.U2RXIF	= 0;
		}
	
	/* Are any Tx interrupts pending? */
	if( IFS1bits.U2TXIF == 1 )
		{
		while( ( U2STAbits.UTXBF ) == 0 )
			{
			Result = QueueReadFromISR( &QueuesTX[0], &cChar );
			if( Result > 0)
				{
				/* Send the next character queued for Tx. */
				U2TXREG = cChar;
				if( Result > 1 )
					MustSwitch	= 1;
				}
			else
				{
				/* Queue empty, nothing to send. */
				IFS1bits.U2TXIF	= 0;
				IEC1bits.U2TXIE	= 0;
				break;
				}
			}
		IFS1bits.U2TXIF	= 0;
		}

	if( MustSwitch )
    	HigherPriorityAwakened();
	}

#endif	/*	defined __DEBUG */
#endif
/*============================================================================*/
#if			defined __DEBUG

static const char	Packet[]	= { 'v', 's', 's', 'k', 'd', 'v', 'v', 'k', 'v', 's', 'k', 'd' };

/*
static const char	Packet[]	= { 0x02, 0x00, 0x01, 0x21, 0x20 };
*/

static int			ip			= 0;

void T2Handler( void )
	{
	int		MustSwitch	= 0;
	char	c			= Packet[ip];

	if( ++ip >= sizeof Packet )
		ip	= 0;

    IFS0bits.T2IF	= 0;

    if( QueueWriteFromISR( &QueuesRX[0], &c ) > 1 )
		MustSwitch	= 1;
    if( QueueReadFromISR( &QueuesTX[0], &c ) > 1 )
    	MustSwitch	= 1;

	if( MustSwitch )
    	HigherPriorityAwakened();
	}

#endif	/*	defined __DEBUG */

/*============================================================================*/
#endif	/*	defined __XC16__ */
/*============================================================================*/

void dummy( void );
