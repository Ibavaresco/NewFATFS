/*============================================================================*/
#if			defined __XC8__ || defined __18CXX
/*============================================================================*/
/* Standard include file. */
#include <stdlib.h>

/*============================================================================*/

/* Scheduler include files. */
#include "Portable.h"
#include "Queue.h"
#include "PortPIC18Internals.h"
//@@@@#include <p18cxxx.h>

/*============================================================================*/

/* Demo app include files. */
#include "SerialPIC18.h"

/*============================================================================*/

/* The queues used to communicate between tasks and ISR's. */
#define QUEUE_LENGTH		32
/*static*/ queue_t			QueuesRX[2]; 
/*static*/ unsigned char	BuffersRX[2][QUEUE_LENGTH]; 
/*static*/ queue_t			QueuesTX[2]; 
/*static*/ unsigned char	BuffersTX[2][QUEUE_LENGTH]; 

/*============================================================================*/
int SerialPortInitMinimal( unsigned int port, unsigned long BaudRate )
	{
	unsigned short	usBRG;

	int				index;

	if( port == 1 )
		index	= 0;
	else if( port == 2 )
		index	= 1;
	else
		return 0;

	/* Create the queues used by the com test task. */
	QueueInit( &QueuesRX[index], sizeof( signed char ), QUEUE_LENGTH, (void*)&BuffersRX, QUEUE_SWITCH_IN_ISR );
	QueueInit( &QueuesTX[index], sizeof( signed char ), QUEUE_LENGTH, (void*)&BuffersTX, QUEUE_NORMAL );

	/* Configure the UART and interrupts. */
	usBRG					= 10000000 / ( 16.0 * BaudRate - 0.5 );

	if( port == 1 )	
		{
		SPBRG1				= usBRG;
		SPBRGH1				= usBRG >> 8;

		LATCbits.LATC6		= 1;

		TRISCbits.TRISC7	= 1;
		TRISCbits.TRISC6	= 0;

		IPR1bits.RC1IP		= 0;
		PIR1bits.RC1IF		= 0;
		IPR1bits.TX1IP		= 0;
		PIR1bits.TX1IF		= 0;

		PIE1bits.RC1IE		= 1;
		PIE1bits.TX1IE		= 0;

		TXSTA1				= 0x20;
		RCSTA1				= 0x90;
		}
	else
		{
		SPBRG2				= usBRG;
		SPBRGH2				= usBRG >> 8;

		LATCbits.LATC6		= 1;

		TRISGbits.TRISG2	= 1;
		TRISCbits.TRISC6	= 0;

		IPR3bits.RC2IP		= 0;
		PIR3bits.RC2IF		= 0;
		IPR3bits.TX2IP		= 0;
		PIR3bits.TX2IF		= 0;

		PIE3bits.RC2IE		= 1;
		PIE3bits.TX2IE		= 0;

		TXSTA2				= 0x20;
		RCSTA2				= 0x90;
		}	
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
			PIE1bits.TX1IE	= 1;

		return 1;
		}
	return 0;
	}

/*============================================================================*/

void U1RXHandler( void )
	{
	int			MustSwitch	= 0;
	/* Declared static to minimise stack use. */
	static char	cChar;
	
	/* Are any Rx interrupts pending? */
	while( PIR1bits.RC1IF == 1 )
		{
		/* Retrieve the received character and place it in the queue of
		received characters. */
		cChar = RCREG1;
		if( QueueWriteFromISR( &QueuesRX[0], &cChar ) > 1 )
			MustSwitch	= 1;
		PIR1bits.RC1IF	= 0;
		}
	
	if( MustSwitch )
    	HigherPriorityAwakened();
	}
/*============================================================================*/

void U1TXHandler( void )
	{
	int			MustSwitch	= 0;
	/* Declared static to minimise stack use. */
	static char	cChar;
	
	/* Are any Tx interrupts pending? */
	while( PIR1bits.TX1IF == 1 )
		{
		int	Result;

		Result = QueueReadFromISR( &QueuesTX[0], &cChar );
		if( Result > 0 )
			{
			/* Send the next character queued for Tx. */
			TXREG1 = cChar;
			if( Result > 1 )
				MustSwitch	= 1;
			}
		else
			{
			/* Queue empty, nothing to send. */
			PIR1bits.TX1IF	= 0;
			PIE1bits.TX1IE	= 0;
			break;
			}
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

    PIR1bits.TMR2IF	= 0;

    if( QueueWriteFromISR( &QueuesRX[0], &c ) > 1 )
		MustSwitch	= 1;
    if( QueueReadFromISR( &QueuesTX[0], &c ) > 1 )
    	MustSwitch	= 1;

	if( MustSwitch )
    	HigherPriorityAwakened();
	}

#endif	/*	defined __DEBUG */

/*============================================================================*/
#endif	/*	defined __XC8__ || defined __18CXX */
/*============================================================================*/
