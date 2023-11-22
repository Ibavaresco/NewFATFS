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

#define	PERIPHERAL_CLOCK_HZ	14745600ul

/*============================================================================*/
int SerialPortInitMinimal( unsigned int port, unsigned long BaudRate )
	{
	int				index;

	if( port == 1 )
		index	= 0;
	else if( port == 2 )
		index	= 1;
	else
		return 0;

	/* Create the queues used by the com test task. */
	QueueInit( &QueuesRX[index], sizeof( signed char ), QUEUE_LENGTH, &BuffersRX, QUEUE_SWITCH_IN_ISR );
	QueueInit( &QueuesTX[index], sizeof( signed char ), QUEUE_LENGTH, &BuffersTX, QUEUE_SWITCH_NORMAL );

#if 1
	/* Configure the UART and interrupts. */

//                            14745600ul / ( 4 * 115200ul ) - 1
	if( port == 1 )	
		{
        // PORTSetPinsDigitalIn( IOPORT_B, BIT_2 );
        ANSBbits.ANSB2		= 0;
        TRISBbits.TRISB2	= 1;

        // PORTSetPinsDigitalOut( IOPORT_B, BIT_7 );
        LATBbits.LATB7		= 1;
        TRISBbits.TRISB7	= 0;

        // PORTSetPinsDigitalOut( IOPORT_B, BIT_9 );
        LATBbits.LATB9		= 1;
        TRISBbits.TRISB9	= 0;

		//U1RXRbits.U1RXR		= 4;
		//RPB3Rbits.RPB3R		= 1;

		//OpenUART1( UART_EN, UART_RX_ENABLE | UART_TX_ENABLE | UART_INT_TX | UART_INT_RX_CHAR, usBRG );
        U1MODEbits.USIDL	= 0;
        U1MODEbits.IREN		= 0;
        U1MODEbits.RTSMD	= 0;
        U1MODEbits.UEN		= 1;
        U1MODEbits.WAKE		= 0;
        U1MODEbits.LPBACK	= 0;
        U1MODEbits.ABAUD	= 0;
        U1MODEbits.RXINV	= 0;
        U1MODEbits.BRGH		= 1;
        U1MODEbits.PDSEL	= 0;
        U1MODEbits.STSEL	= 1;    /* Two stop bits (for MODBUS) */

        U1STAbits.UTXISEL0	= 0;
        U1STAbits.UTXISEL1	= 0;
        U1STAbits.UTXINV	= 0;
        U1STAbits.UTXBRK	= 0;
        U1STAbits.UTXEN		= 1;
        U1STAbits.URXISEL	= 0;
        U1STAbits.ADDEN		= 0;
        U1STAbits.OERR		= 0;

    	U1BRG				= (unsigned short)(( (float)PERIPHERAL_CLOCK_HZ / ( (float)4 * (float)BaudRate ) ) - (float)0.5);

    	U1MODEbits.UARTEN	= 1;

		//INTSetVectorPriority( INT_UART_1_VECTOR, KERNEL_INTERRUPT_PRIORITY + 1 );
        IPC2bits.U1RXIP     = 1;
		//INTClearFlag( INT_U1RX );
        IFS0bits.U1RXIF		= 0;
		//INTEnable( INT_U1RX, INT_ENABLED );		
        IEC0bits.U1RXIE		= 1;

		//INTSetVectorSubPriority( INT_UART_1_VECTOR, UART_INT_SUB_PR0 );
        IPC3bits.U1TXIP     = 1;
		//INTClearFlag( INT_U1TX );
        IFS0bits.U1TXIF		= 0;
		//INTEnable( INT_U1TX, INT_DISABLED );
        IEC0bits.U1TXIE		= 0;
		}
	else
		{
/*
		PORTSetPinsDigitalIn( IOPORT_B, BIT_8 );
		PORTSetPinsDigitalOut( IOPORT_B, BIT_9 );
		U2RXRbits.U2RXR		= 4;
		RPB9Rbits.RPB9R		= 2;

		OpenUART2( UART_EN, UART_RX_ENABLE | UART_TX_ENABLE | UART_INT_TX | UART_INT_RX_CHAR, usBRG );

		INTClearFlag( INT_U2RX );
		INTClearFlag( INT_U2TX );
		INTSetVectorPriority( INT_UART_2_VECTOR, KERNEL_INTERRUPT_PRIORITY + 1 );
		INTSetVectorSubPriority( INT_UART_2_VECTOR, UART_INT_SUB_PR0 );
		INTEnable( INT_U2RX, INT_ENABLED );
		INTEnable( INT_U2TX, INT_DISABLED );
*/
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

		if( index )
			IEC1bits.U2TXIE	= 1;
		else
			IEC0bits.U1TXIE	= 1;

		return 1;
		}
	return 0;
	}

/*============================================================================*/
void __attribute__((noinline)) U1RXHandler( void )
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
	if( MustSwitch )
    	HigherPriorityAwakened();
    }
/*============================================================================*/
void __attribute__((noinline)) U1TXHandler( void )
	{
	/* Declared static to minimise stack use. */
	static char	cChar;

	/* Are any Tx interrupts pending? */
	if( IFS0bits.U1TXIF == 1 )
		{
		while( ( U1STAbits.UTXBF ) == 0 )
			{
			int	Result;

			Result = QueueReadFromISR( &QueuesTX[0], &cChar );
			if( Result > 0 )
				/* Send the next character queued for Tx. */
				U1TXREG = cChar;
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
	}
/*============================================================================*/
void __attribute__((interrupt,naked,auto_psv)) _U1RXInterrupt( void )
	{
	SaveContext();

	U1RXHandler();

	/*
	Doing it this way to save stack and also because some dsPICs and PIC24 execute
	'bra' and 'goto' faster than 'call' or 'rcall'. We are not returnig from here
	anyway.
	*/
	asm volatile( "bra  _RestoreContext	\n" );
    }
/*============================================================================*/
void __attribute__((interrupt,naked,auto_psv)) _U1TXInterrupt( void )
	{
	SaveContext();

	U1TXHandler();

	/*
	Doing it this way to save stack and also because some dsPICs and PIC24 execute
	'bra' and 'goto' faster than 'call' or 'rcall'. We are not returnig from here
	anyway.
	*/
	asm volatile( "bra  _RestoreContext	\n" );
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
