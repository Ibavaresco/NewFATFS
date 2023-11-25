/*============================================================================*/
#if			defined __XC32__ || defined __C32_VERSION__
/*============================================================================*/

#include <stdlib.h>

/*============================================================================*/

/* Scheduler include files. */
#include "Portable.h"
#include "Queue.h"
#include "PortPIC32Internals.h"
#include "SimpleRTOSInternals.h"

/*============================================================================*/

/* Demo app include files. */
#include "SerialPIC32.h"

/*============================================================================*/
#if			defined __PIC32MX__
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#if			defined __32MX110F016B__ || defined __32MX110F016C__ || defined __32MX110F016D__ || \
			defined __32MX120F032B__ || defined __32MX120F032C__ || defined __32MX120F032D__ || \
			defined __32MX130F064B__ || defined __32MX130F064C__ || defined __32MX130F064D__ || \
			defined __32MX150F128B__ || defined __32MX150F128C__ || defined __32MX150F128D__ || \
			defined __32MX170F256B__ ||								defined __32MX170F256D__ || \
			defined __32MX210F016B__ || defined __32MX210F016C__ || defined __32MX210F016D__ || \
			defined __32MX220F032B__ || defined __32MX220F032C__ || defined __32MX220F032D__ || \
			defined __32MX230F064B__ || defined __32MX230F064C__ || defined __32MX230F064D__ || \
			defined __32MX250F128B__ || defined __32MX250F128C__ || defined __32MX250F128D__ || \
			defined __32MX270F256B__ ||								defined __32MX270F256D__
/*----------------------------------------------------------------------------*/
#define	SERIAL_PORTS	 2

#define	IFSbitsU1RX	IFS1bits
#define	IFSbitsU1TX	IFS1bits
#define	IECbitsU1RX	IEC1bits
#define	IECbitsU1TX	IEC1bits
#define	IPCbitsU1	IPC8bits

#define	IFSbitsU2RX	IFS1bits
#define	IFSbitsU2TX	IFS1bits
#define	IECbitsU2RX	IEC1bits
#define	IECbitsU2TX	IEC1bits
#define	IPCbitsU2	IPC9bits
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#elif		defined __32MX320F032H__ || defined __32MX320F064H__ || defined __32MX320F128H__ || \
			defined __32MX340F128H__ || defined __32MX340F256H__ || defined __32MX340F512H__ || \
			defined __32MX320F128L__ || \
			defined __32MX340F128L__ || \
			defined __32MX360F256L__ || defined __32MX360F512L__ || \
			defined __32MX420F032H__ || \
			defined __32MX440F128H__ || defined __32MX440F256H__ || defined __32MX440F512H__ || \
			defined __32MX440F128L__ || \
			defined __32MX460F256L__ || defined __32MX460F512L__
/*----------------------------------------------------------------------------*/
#define	SERIAL_PORTS	 2

#define	IFSbitsU1RX	IFS0bits
#define	IFSbitsU1TX	IFS0bits
#define	IECbitsU1RX	IEC0bits
#define	IECbitsU1TX	IEC0bits
#define	IPCbitsU1	IPC6bits

#define	IFSbitsU2RX	IFS1bits
#define	IFSbitsU2TX	IFS1bits
#define	IECbitsU2RX	IEC1bits
#define	IECbitsU2TX	IEC1bits
#define	IPCbitsU2	IPC8bits
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#elif		defined __32MX120F064H__ ||	defined __32MX130F128H__ ||	\
			defined __32MX150F256H__ || defined __32MX170F512H__ ||	\
			defined __32MX230F128H__ || defined __32MX250F256H__ ||	\
			defined __32MX270F512H__ ||								\
			defined __32MX330F064H__ || defined __32MX350F128H__ ||	\
			defined __32MX350F256H__ || defined __32MX370F512H__ ||	\
			defined __32MX430F064H__ || defined __32MX450F128H__ ||	\
			defined __32MX450F256H__ || defined __32MX470F512H__ || \
																	\
			defined __32MX530F128H__ || defined __32MX550F256H__ || defined __32MX570F512H__

/*----------------------------------------------------------------------------*/
#define	SERIAL_PORTS	 4
			
#define	IFSbitsU1RX	IFS1bits
#define	IFSbitsU1TX	IFS1bits
#define	IECbitsU1RX	IEC1bits
#define	IECbitsU1TX	IEC1bits
#define	IPCbitsU1	IPC7bits

#define	IFSbitsU2RX	IFS1bits
#define	IFSbitsU2TX	IFS1bits
#define	IECbitsU2RX	IEC1bits
#define	IECbitsU2TX	IEC1bits
#define	IPCbitsU2	IPC9bits

#define	IFSbitsU3RX	IFS1bits
#define	IFSbitsU3TX	IFS2bits
#define	IECbitsU3RX	IEC1bits
#define	IECbitsU3TX	IEC2bits
#define	IPCbitsU3	IPC9bits

#define	IFSbitsU4RX	IFS2bits
#define	IFSbitsU4TX	IFS2bits
#define	IECbitsU4RX	IEC2bits
#define	IECbitsU4TX	IEC2bits
#define	IPCbitsU4	IPC9bits
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#elif		defined __32MX130F128L__ || defined __32MX150F256L__ ||	\
			defined __32MX170F512L__ || defined __32MX230F128L__ ||	\
			defined __32MX250F256L__ || defined __32MX270F512L__ ||	\
																	\
			defined __32MX330F064L__ || defined __32MX350F128L__ ||	\
			defined __32MX350F256L__ || defined __32MX370F512L__ ||	\
			defined __32MX430F064L__ || defined __32MX450F128L__ ||	\
			defined __32MX450F256L__ || defined __32MX470F512L__ || \
																	\
			defined __32MX530F128L__ || defined __32MX550F256L__ || defined __32MX570F512L__

/*----------------------------------------------------------------------------*/
#define	SERIAL_PORTS	 5

#define	IFSbitsU1RX	IFS1bits
#define	IFSbitsU1TX	IFS1bits
#define	IECbitsU1RX	IEC1bits
#define	IECbitsU1TX	IEC1bits
#define	IPCbitsU1	IPC7bits

#define	IFSbitsU2RX	IFS1bits
#define	IFSbitsU2TX	IFS1bits
#define	IECbitsU2RX	IEC1bits
#define	IECbitsU2TX	IEC1bits
#define	IPCbitsU2	IPC9bits

#define	IFSbitsU3RX	IFS1bits
#define	IFSbitsU3TX	IFS2bits
#define	IECbitsU3RX	IEC1bits
#define	IECbitsU3TX	IEC2bits
#define	IPCbitsU3	IPC9bits

#define	IFSbitsU4RX	IFS2bits
#define	IFSbitsU4TX	IFS2bits
#define	IECbitsU4RX	IEC2bits
#define	IECbitsU4TX	IEC2bits
#define	IPCbitsU4	IPC9bits

#define	IFSbitsU5RX	IFS2bits
#define	IFSbitsU5TX	IFS2bits
#define	IECbitsU5RX	IEC2bits
#define	IECbitsU5TX	IEC2bits
#define	IPCbitsU5	IPC10bits
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#elif		defined __32MX534F064H__ || defined __32MX534F064L__ || \
			defined __32MX564F064H__ || defined __32MX564F064L__ || \
			defined __32MX564F128H__ || defined __32MX564F128L__ || \
			defined __32MX575F256H__ || defined __32MX575F256L__ || \
			defined __32MX575F512H__ || defined __32MX575F512L__ || \
			defined __32MX664F064H__ || defined __32MX664F064L__ || \
			defined __32MX664F128H__ || defined __32MX664F128L__ || \
			defined __32MX675F256H__ || defined __32MX675F256L__ || \
			defined __32MX675F512H__ || defined __32MX675F512L__ || \
			defined __32MX695F512H__ || defined __32MX695F512L__ || \
			defined __32MX764F128H__ || defined __32MX764F128L__ || \
			defined __32MX775F256H__ || defined __32MX775F256L__ || \
			defined __32MX775F512H__ || defined __32MX775F512L__ || \
			defined __32MX795F512H__ || defined __32MX795F512L__
/*----------------------------------------------------------------------------*/
#define	SERIAL_PORTS	 6

#define	IFSbitsU1RX	IFS0bits
#define	IFSbitsU1TX	IFS0bits
#define	IECbitsU1RX	IEC0bits
#define	IECbitsU1TX	IEC0bits
#define	IPCbitsU1	IPC6bits

#define	IFSbitsU2RX	IFS1bits
#define	IFSbitsU2TX	IFS1bits
#define	IECbitsU2RX	IEC1bits
#define	IECbitsU2TX	IEC1bits
#define	IPCbitsU2	IPC8bits

#define	IFSbitsU3RX	IFS1bits
#define	IFSbitsU3TX	IFS1bits
#define	IECbitsU3RX	IEC1bits
#define	IECbitsU3TX	IEC1bits
#define	IPCbitsU3	IPC7bits

#define	IFSbitsU4RX	IFS2bits
#define	IFSbitsU4TX	IFS2bits
#define	IECbitsU4RX	IEC2bits
#define	IECbitsU4TX	IEC2bits
#define	IPCbitsU4	IPC12bits

#define	IFSbitsU5RX	IFS2bits
#define	IFSbitsU5TX	IFS2bits
#define	IECbitsU5RX	IEC2bits
#define	IECbitsU5TX	IEC2bits
#define	IPCbitsU5	IPC12bits

#define	IFSbitsU6RX	IFS2bits
#define	IFSbitsU6TX	IFS2bits
#define	IECbitsU6RX	IEC2bits
#define	IECbitsU6TX	IEC2bits
#define	IPCbitsU6	IPC12bits
/*----------------------------------------------------------------------------*/
#endif	/*	defined __32MX110F016B__ ... */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#elif		defined __PIC32MK__ 

#define	SERIAL_PORTS	 6

#define	IFSbitsU1RX	IFS1bits
#define	IFSbitsU1TX	IFS1bits
#define	IFSbitsU2RX	IFS1bits
#define	IFSbitsU2TX	IFS1bits
#define	IFSbitsU3RX	IFS1bits
#define	IFSbitsU3TX	IFS2bits
#define	IFSbitsU4RX	IFS2bits
#define	IFSbitsU4TX	IFS2bits
#define	IFSbitsU5RX	IFS2bits
#define	IFSbitsU5TX	IFS2bits
#define	IFSbitsU6RX	IFS5bits
#define	IFSbitsU6TX	IFS5bits

#define	IECbitsU1RX	IEC1bits
#define	IECbitsU1TX	IEC1bits
#define	IECbitsU2RX	IEC1bits
#define	IECbitsU2TX	IEC1bits
#define	IECbitsU3RX	IEC1bits
#define	IECbitsU3TX	IEC2bits
#define	IECbitsU4RX	IEC2bits
#define	IECbitsU4TX	IEC2bits
#define	IECbitsU5RX	IEC2bits
#define	IECbitsU5TX	IEC2bits
#define	IECbitsU6RX	IEC5bits
#define	IECbitsU6TX	IEC5bits

#define	IPCbitsU1RX	IPC9bits
#define	IPCbitsU1TX	IPC10bits
#define	IPCbitsU2RX	IPC14bits
#define	IPCbitsU2TX	IPC14bits
#define	IPCbitsU3RX	IPC15bits
#define	IPCbitsU3TX	IPC16bits
#define	IPCbitsU4RX	IPC16bits
#define	IPCbitsU4TX	IPC16bits
#define	IPCbitsU5RX	IPC17bits
#define	IPCbitsU5TX	IPC17bits
#define	IPCbitsU6RX	IPC41bits
#define	IPCbitsU6TX	IPC41bits

#elif		defined __PIC32MZ__

#define	SERIAL_PORTS	 6

#define	IFSbitsU1RX	IFS3bits
#define	IFSbitsU1TX	IFS3bits
#define	IFSbitsU2RX	IFS4bits
#define	IFSbitsU2TX	IFS4bits
#define	IFSbitsU3RX	IFS4bits
#define	IFSbitsU3TX	IFS4bits
#define	IFSbitsU4RX	IFS5bits
#define	IFSbitsU4TX	IFS5bits
#define	IFSbitsU5RX	IFS5bits
#define	IFSbitsU5TX	IFS5bits
#define	IFSbitsU6RX	IFS5bits
#define	IFSbitsU6TX	IFS5bits

#define	IECbitsU1RX	IEC3bits
#define	IECbitsU1TX	IEC3bits
#define	IECbitsU2RX	IEC4bits
#define	IECbitsU2TX	IEC4bits
#define	IECbitsU3RX	IEC4bits
#define	IECbitsU3TX	IEC4bits
#define	IECbitsU4RX	IEC5bits
#define	IECbitsU4TX	IEC5bits
#define	IECbitsU5RX	IEC5bits
#define	IECbitsU5TX	IEC5bits
#define	IECbitsU6RX	IEC5bits
#define	IECbitsU6TX	IEC5bits

#define	IPCbitsU1RX	IPC28bits
#define	IPCbitsU1TX	IPC28bits
#define	IPCbitsU2RX	IPC36bits
#define	IPCbitsU2TX	IPC36bits
#define	IPCbitsU3RX	IPC39bits
#define	IPCbitsU3TX	IPC39bits
#define	IPCbitsU4RX	IPC42bits
#define	IPCbitsU4TX	IPC43bits
#define	IPCbitsU5RX	IPC45bits
#define	IPCbitsU5TX	IPC45bits
#define	IPCbitsU6RX	IPC47bits
#define	IPCbitsU6TX	IPC47bits

#endif	/*	defined __PIC32MX__ */
/*============================================================================*/

#define QUEUE_LENGTH	128

/*============================================================================*/

/* The queues used to communicate between tasks and ISR's. */
/*static*/ queue_t			QueuesRX[SERIAL_PORTS];
/*static*/ unsigned char	BuffersRX[SERIAL_PORTS][QUEUE_LENGTH];
/*static*/ queue_t			QueuesTX[SERIAL_PORTS];
/*static*/ unsigned char	BuffersTX[SERIAL_PORTS][QUEUE_LENGTH];

/* Flag used to indicate the tx status. */

#define	PERIPHERAL_CLOCK_HZ	60000000

/*============================================================================*/
#if			defined __PIC32MX__

#define	SERIALINIT(p)       				\
	U##p##MODEbits.ON       	= 0;		\
	U##p##BRG               	= usBRG;	\
/*	U##p##MODEbits.UEN      	= 0; */		\
	U##p##MODEbits.BRGH     	= 1;		\
	U##p##MODEbits.ON       	= 1;		\
	U##p##STAbits.UTXISEL   	= 0;		\
	U##p##STAbits.URXEN     	= 1;		\
	U##p##STAbits.UTXEN     	= 1;		\
	U##p##STAbits.URXISEL   	= 0;		\
	IFSbitsU##p##RX.U##p##RXIF	= 0;		\
	IPCbitsU##p.U##p##IP    	= KERNEL_INTERRUPT_PRIORITY + 0;	\
	IPCbitsU##p.U##p##IS    	= 0;		\
	IECbitsU##p##RX.U##p##RXIE	= 1;

#elif		defined __PIC32MK__ || defined __PIC32MZ__

#define	SERIALINIT(p)					\
	U##p##MODEbits.ON		= 0;		\
	U##p##BRG				= usBRG;	\
/*	U##p##MODEbits.UEN		= 0; */		\
	U##p##MODEbits.BRGH		= 1;		\
	U##p##MODEbits.ON		= 1;		\
	U##p##STAbits.UTXISEL	= 0;		\
	U##p##STAbits.URXEN		= 1;		\
	U##p##STAbits.UTXEN		= 1;		\
	U##p##STAbits.URXISEL	= 0;		\
	IFSbitsU##p##RX.U##p##RXIF	= 0;		\
	IPCbitsU##p##RX.U##p##RXIP	= KERNEL_INTERRUPT_PRIORITY + 0;	\
	IPCbitsU##p##TX.U##p##TXIP	= KERNEL_INTERRUPT_PRIORITY + 0;	\
	IPCbitsU##p##RX.U##p##RXIS	= 0;		\
	IPCbitsU##p##TX.U##p##TXIS	= 0;		\
	IECbitsU##p##RX.U##p##RXIE	= 1;

#endif	/*	defined __PIC32MX__ */
/*============================================================================*/
extern __attribute__((weak)) void UserConfigUART1( void );
extern __attribute__((weak)) void UserConfigUART2( void );
extern __attribute__((weak)) void UserConfigUART3( void );
extern __attribute__((weak)) void UserConfigUART4( void );
extern __attribute__((weak)) void UserConfigUART5( void );
extern __attribute__((weak)) void UserConfigUART6( void );
/*============================================================================*/
int SerialPortInitMinimal( unsigned int port, unsigned long BaudRate )
	{
	unsigned short	usBRG;

	if( port < 1 || port > SERIAL_PORTS )
		return 0;

	/* Create the queues used by the com test task. */
	QueueInit( &QueuesRX[port-1], sizeof( signed char ), QUEUE_LENGTH, &BuffersRX[port-1], QUEUE_SWITCH_IN_ISR );
	QueueInit( &QueuesTX[port-1], sizeof( signed char ), QUEUE_LENGTH, &BuffersTX[port-1], QUEUE_SWITCH_NORMAL );

	/* Configure the UART and interrupts. */
	usBRG				= (unsigned short)(( (float)PERIPHERAL_CLOCK_HZ / ( (float)4 * (float)BaudRate ) ) - (float)0.5);

	switch( port )
		{
		case 1:
			SERIALINIT( 1 );
            if( UserConfigUART1 != NULL )
                UserConfigUART1();
			break;
#if			SERIAL_PORTS >= 2
		case 2:
#if			0
 			SERIALINIT( 2 );
            if( UserConfigUART2 != NULL )
                UserConfigUART2();
#else 	/*	0 */        
			U2MODEbits.ON		= 0;
			U2BRG				= usBRG;
		/*	U2MODEbits.UEN		= 0; */
			U2MODEbits.BRGH		= 1;
			U2MODEbits.ON		= 1;
			U2STAbits.UTXISEL	= 0;
			U2STAbits.URXEN		= 1;
			U2STAbits.UTXEN		= 1;
			U2STAbits.URXISEL	= 0;
			IFSbitsU2RX.U2RXIF	= 0;
#if         defined __PIC32MK__ || defined __PIC32MZ__
			IPCbitsU2RX.U2RXIP	= KERNEL_INTERRUPT_PRIORITY + 0;
			IPCbitsU2RX.U2RXIS	= 0;
			IPCbitsU2TX.U2TXIP	= KERNEL_INTERRUPT_PRIORITY + 0;
			IPCbitsU2TX.U2TXIS	= 0;
			IECbitsU2RX.U2RXIE	= 1;
#else   /*  defined __PIC32MK__ || defined __PIC32MZ__ */
			IPCbitsU2.U2IP  	= KERNEL_INTERRUPT_PRIORITY + 0;
			IPCbitsU2.U2IS  	= 0;
			IECbitsU2RX.U2RXIE	= 1;
#endif  /*  defined __PIC32MK__ || defined __PIC32MZ__ */
            if( UserConfigUART2 != NULL )
                UserConfigUART2();
#endif	/* 0 */
            break;
#endif	/*	SERIAL_PORTS >= 2 */
#if			SERIAL_PORTS >= 3
		case 3:
			SERIALINIT( 3 );
            if( UserConfigUART3 != NULL )
                UserConfigUART3();
			break;
#endif	/*	SERIAL_PORTS >= 3 */
#if			SERIAL_PORTS >= 4
		case 4:
			SERIALINIT( 4 );
            if( UserConfigUART4 != NULL )
                UserConfigUART4();
			break;
#endif	/*	SERIAL_PORTS >= 4 */
#if			SERIAL_PORTS >= 5
		case 5:
			SERIALINIT( 5 );
            if( UserConfigUART5 != NULL )
                UserConfigUART5();
			break;
#endif	/*	SERIAL_PORTS >= 5 */
#if			SERIAL_PORTS >= 6
		case 6:
			SERIALINIT( 6 );
            if( UserConfigUART6 != NULL )
                UserConfigUART6();
			break;
#endif	/*	SERIAL_PORTS >= 6 */
		}

	return 1;
	}

/*============================================================================*/

int SerialReceive( unsigned int port, tickcount_t TimeToWait )
	{
	unsigned char	c;

	if( port < 1 || port > SERIAL_PORTS )
		return -2;

	if( QueueRead( &QueuesRX[port-1], &c, TimeToWait ))
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
	if( port < 1 || port > SERIAL_PORTS )
		return -2;

	/* Return false if after the block time there is no room on the Tx queue. */
	if( QueueWrite( &QueuesTX[port-1], &c, TimeToWait ) == 0 )
		return 0;
	else
		{
		if( port == 1 )
			IECbitsU1TX.U1TXIE	= 1;
#if			SERIAL_PORTS >= 2
		else if( port == 2 )
			IECbitsU2TX.U2TXIE	= 1;
#endif	/*	SERIAL_PORTS >= 2 */
#if			SERIAL_PORTS >= 3
		else if( port == 3 )
			IECbitsU3TX.U3TXIE	= 1;
#endif	/*	SERIAL_PORTS >= 3 */
#if			SERIAL_PORTS >= 4
		else if( port == 4 )
			IECbitsU4TX.U4TXIE	= 1;
#endif	/*	SERIAL_PORTS >= 4 */
#if			SERIAL_PORTS >= 5
		else if( port == 5 )
			IECbitsU5TX.U5TXIE	= 1;
#endif	/*	SERIAL_PORTS >= 5 */
#if			SERIAL_PORTS >= 6
		else if( port == 6 )
			IECbitsU6TX.U6TXIE	= 1;
#endif	/*	SERIAL_PORTS >= 6 */

		return 1;
		}
	return 0;
	}

/*============================================================================*/

#define IMPLEMENTVECTOR(p,v,n)												\
void __attribute__(( vector( _UART##v##_VECTOR ), interrupt, nomips16, naked )) U##n##Vector( void )	\
	{																		\
	asm volatile(															\
		".extern	SaveContext		\r\n"									\
		".extern	RestoreContext	\r\n"									\
																			\
		"addiu		$sp,$sp,-8		\r\n"									\
		"sw			$ra,4($sp)		\r\n"									\
																			\
		"jal		SaveContext		\r\n"									\
		"nop						\r\n"									\
		/*------------------------------------------------------------*/	\
		"jal		U" #p "Handler	\r\n"									\
		"nop						\r\n"									\
		/*------------------------------------------------------------*/	\
		"j			RestoreContext	\r\n"									\
		"nop						\r\n"									\
		/*------------------------------------------------------------*/	\
		);																	\
	}

/*============================================================================*/

#define	IMPLEMENTRXHANDLER(p)												\
	/* Are any Rx interrupts pending? */									\
	if( U##p##STAbits.OERR != 0 )											\
		{																	\
		U##p##STAbits.OERR	= 0;											\
		U##p##Overrun++;													\
		}																	\
																			\
	if( IFSbitsU##p##RX.U##p##RXIF == 1 && IECbitsU##p##RX.U##p##RXIE == 1 )\
		{																	\
		while( U##p##STAbits.URXDA )										\
			{																\
			/* Retrieve the received character and place it in the queue of	\
			received characters. */											\
			cChar = U##p##RXREG;											\
			if( QueueWriteFromISR( &QueuesRX[p-1], &cChar ) > 1 )			\
				MustSwitch	= 1;											\
			}																\
		IFSbitsU##p##RX.U##p##RXIF	= 0;										\
		}

/*============================================================================*/

#define	IMPLEMENTTXHANDLER(p)												\
	/* Are any Tx interrupts pending? */									\
	if( IFSbitsU##p##TX.U##p##TXIF == 1 && IECbitsU##p##TX.U##p##TXIE == 1 )\
		{																	\
		while( U##p##STAbits.UTXBF == 0 )									\
			{																\
			int	Result;														\
																			\
			Result = QueueReadFromISR( &QueuesTX[p-1], &cChar );			\
			if( Result > 0 )												\
				{															\
				/* Send the next character queued for Tx. */				\
				U##p##TXREG = cChar;										\
				if( Result > 1 )											\
					MustSwitch	= 1;										\
				}															\
			else															\
				{															\
				/* Queue empty, nothing to send. */							\
				/*IFSbitsU##p.U##p##TXIF	= 0;*/							\
				IECbitsU##p##TX.U##p##TXIE	= 0;								\
				break;														\
				}															\
			}																\
																			\
		if( U##p##STAbits.UTXBF != 0 )										\
			IFSbitsU##p##TX.U##p##TXIF	= 0;									\
		}

/*============================================================================*/
#if			defined __PIC32MX__
/*============================================================================*/

#define	DECLAREVECTOR(p)													\
	IMPLEMENTVECTOR(p,_##p,p)

#define	DECLAREHANDLER(p)													\
unsigned long	U##p##Overrun = 0;											\
void U##p##Handler( void )													\
	{																		\
	int			MustSwitch	= 0;											\
	static char	cChar;														\
																			\
	IMPLEMENTRXHANDLER(p)													\
	IMPLEMENTTXHANDLER(p)													\
																			\
	if( MustSwitch )														\
		CurrentTask	= ReadyTasks[HighestReadyPriority];						\
	}

/*============================================================================*/
#elif		defined __PIC32MK__ || defined __PIC32MZ__
/*============================================================================*/

#define	DECLAREVECTOR(p)													\
	IMPLEMENTVECTOR(p##_RX,p##_RX,p##RX)									\
	IMPLEMENTVECTOR(p##_TX,p##_TX,p##TX)

#define	DECLAREHANDLER(p)													\
unsigned long	U##p##Overrun	= 0;										\
void U##p##_RXHandler( void )												\
	{																		\
	int			MustSwitch	= 0;											\
	static char	cChar;														\
																			\
	IMPLEMENTRXHANDLER(p)													\
																			\
	if( MustSwitch )														\
		CurrentTask	= ReadyTasks[HighestReadyPriority];						\
	}																		\
																			\
void U##p##_TXHandler( void )												\
	{																		\
	int			MustSwitch	= 0;											\
	static char	cChar;														\
																			\
	IMPLEMENTTXHANDLER(p)													\
																			\
	if( MustSwitch )														\
		CurrentTask	= ReadyTasks[HighestReadyPriority];						\
	}

/*============================================================================*/
#endif	/*	defined __PIC32MX__ */
/*============================================================================*/

#if			SERIAL_PORTS >= 1
DECLAREVECTOR(1)
DECLAREHANDLER(1)
#endif	/*	SERIAL_PORTS >= 1 */

#if			SERIAL_PORTS >= 2
#if			0
DECLAREVECTOR(2)
DECLAREHANDLER(2)
#else	/*	0 */

#if         defined __PIC32MK__ || defined __PIC32MZ__
void __attribute__(( vector( _UART2_RX_VECTOR ), interrupt, nomips16, naked )) U2RXVector( void )
	{
	asm volatile(
		".extern	SaveContext		\r\n"
		".extern	RestoreContext	\r\n"

		"addiu		$sp,$sp,-8		\r\n"
		"sw			$ra,4($sp)		\r\n"

		"jal		SaveContext		\r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		"jal		U2RXHandler     \r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		"j			RestoreContext	\r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		);
	}

void __attribute__(( vector( _UART2_TX_VECTOR ), interrupt, nomips16, naked )) U2TXVector( void )
	{
	asm volatile(
		".extern	SaveContext		\r\n"
		".extern	RestoreContext	\r\n"

		"addiu		$sp,$sp,-8		\r\n"
		"sw			$ra,4($sp)		\r\n"

		"jal		SaveContext		\r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		"jal		U2TXHandler     \r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		"j			RestoreContext	\r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		);
	}

unsigned long	U2Overrun	= 0;
void U2RXHandler( void )
	{
	int			MustSwitch	= 0;
	static char	cChar;

	if( U2STAbits.OERR != 0 )
		{
		U2STAbits.OERR	= 0;
		U2Overrun++;
		}

	/* Are any Rx interrupts pending? */
	if( IFSbitsU2RX.U2RXIF == 1 && IECbitsU2RX.U2RXIE == 1 )
		{
		while( U2STAbits.URXDA )
			{
			/* Retrieve the received character and place it in the queue of
			received characters. */
			cChar = U2RXREG;
			if( QueueWriteFromISR( &QueuesRX[2-1], &cChar ) > 1 )
				MustSwitch	= 1;
			}
		IFSbitsU2RX.U2RXIF	= 0;
		}

	if( MustSwitch )
		CurrentTask	= ReadyTasks[HighestReadyPriority];
	}

void U2TXHandler( void )
	{
	int			MustSwitch	= 0;
	static char	cChar;

	/* Are any Tx interrupts pending? */
	if( IFSbitsU2TX.U2TXIF == 1 && IECbitsU2TX.U2TXIE == 1 )
		{
		while( U2STAbits.UTXBF == 0 )
			{
			int	Result;

			Result = QueueReadFromISR( &QueuesTX[2-1], &cChar );
			if( Result > 0 )
				{
				/* Send the next character queued for Tx. */
				U2TXREG = cChar;
				if( Result > 1 )
					MustSwitch	= 1;
				}
			else
				{
				/* Queue empty, nothing to send. */
				/*IFSbitsU##p.U##p##TXIF	= 0;*/
				IECbitsU2TX.U2TXIE	= 0;
				break;
				}
			}

		if( U2STAbits.UTXBF != 0 )
			IFSbitsU2TX.U2TXIF	= 0;
		}

	if( MustSwitch )
		CurrentTask	= ReadyTasks[HighestReadyPriority];
	}

#if 0
void __attribute__(( vector( _UART5_RX_VECTOR ), interrupt, nomips16, naked )) U5RXVector( void )
	{
	asm volatile(
		".extern	SaveContext		\r\n"
		".extern	RestoreContext	\r\n"

		"addiu		$sp,$sp,-8		\r\n"
		"sw			$ra,4($sp)		\r\n"

		"jal		SaveContext		\r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		"jal		U5RXHandler     \r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		"j			RestoreContext	\r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		);
	}

void __attribute__(( vector( _UART5_TX_VECTOR ), interrupt, nomips16, naked )) U5TXVector( void )
	{
	asm volatile(
		".extern	SaveContext		\r\n"
		".extern	RestoreContext	\r\n"

		"addiu		$sp,$sp,-8		\r\n"
		"sw			$ra,4($sp)		\r\n"

		"jal		SaveContext		\r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		"jal		U5TXHandler     \r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		"j			RestoreContext	\r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		);
	}

unsigned long	U5Overrun	= 0;
void U5RXHandler( void )
	{
	int			MustSwitch	= 0;
	static char	cChar;

	if( U5STAbits.OERR != 0 )
		{
		U5STAbits.OERR	= 0;
		U5Overrun++;
		}

	if( IFSbitsU5RX.U5RXIF == 1 && IECbitsU5RX.U5RXIE == 1 )
		{
		while( U5STAbits.URXDA )
			{
			/* Retrieve the received character and place it in the queue of	
			received characters. */
			cChar = U5RXREG;
			if( QueueWriteFromISR( &QueuesRX[5-1], &cChar ) > 1 )
				MustSwitch	= 1;
			}
		IFSbitsU5RX.U5RXIF	= 0;
		}

	if( MustSwitch )
		CurrentTask	= ReadyTasks[HighestReadyPriority];
	}

void U5TXHandler( void )
	{
	int			MustSwitch	= 0;
	static char	cChar;

    /* Are any Tx interrupts pending? */
	if( IFSbitsU5TX.U5TXIF == 1 && IECbitsU5TX.U5TXIE == 1 )
		{
		while( U5STAbits.UTXBF == 0 )
			{
			int	Result;

			Result = QueueReadFromISR( &QueuesTX[5-1], &cChar );
			if( Result > 0 )
				{
				/* Send the next character queued for Tx. */
				U5TXREG = cChar;

				T9CONbits.ON	= 0;
				TMR9		   -= 4790;
				IFS2bits.T9IF	= 0;
				T9CONbits.ON	= 1;
				LATAbits.LATA15	= 1;
				if( Result > 1 )
					MustSwitch	= 1;
				}
			else
				{
				/* Queue empty, nothing to send. */
				/*IFSbitsU##p.U##p##TXIF	= 0;*/
				IECbitsU5TX.U5TXIE	= 0;
				break;
				}
			}

		if( U5STAbits.UTXBF != 0 )
			IFSbitsU5TX.U5TXIF	= 0;
		}

	if( MustSwitch )
		CurrentTask	= ReadyTasks[HighestReadyPriority];
	}
#endif
#else   /*  defined __PIC32MK__ || defined __PIC32MZ__ */
void __attribute__(( vector( _UART_2_VECTOR ), interrupt, nomips16, naked )) U2Vector( void )
	{
	asm volatile(
		".extern	SaveContext		\r\n"
		".extern	RestoreContext	\r\n"

		"addiu		$sp,$sp,-8		\r\n"
		"sw			$ra,4($sp)		\r\n"

		"jal		SaveContext		\r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		"jal		U2Handler       \r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		"j			RestoreContext	\r\n"
		"nop						\r\n"
		/*------------------------------------------------------------*/
		);
	}

unsigned long	U2Overrun	= 0;
void U2Handler( void )
	{
	int			MustSwitch	= 0;
	static char	cChar;

	if( U2STAbits.OERR != 0 )
		{
		U2STAbits.OERR	= 0;
		U2Overrun++;
		}

	if( IFSbitsU2RX.U2RXIF == 1 && IECbitsU2RX.U2RXIE == 1 )
		{
		while( U2STAbits.URXDA )
			{
			/* Retrieve the received character and place it in the queue of	
			received characters. */
			cChar = U2RXREG;
			if( QueueWriteFromISR( &QueuesRX[2-1], &cChar ) > 1 )
				MustSwitch	= 1;
			}
		IFSbitsU2RX.U2RXIF	= 0;
		}

    /* Are any Tx interrupts pending? */
	if( IFSbitsU2TX.U2TXIF == 1 && IECbitsU2TX.U2TXIE == 1 )
		{
		while( U2STAbits.UTXBF == 0 )
			{
			int	Result;
			Result = QueueReadFromISR( &QueuesTX[2-1], &cChar );
			if( Result > 0 )
				{
				/* Send the next character queued for Tx. */
				U2TXREG = cChar;
				if( Result > 1 )
					MustSwitch	= 1;
				}
			else
				{
				/* Queue empty, nothing to send. */
				/*IFSbitsU##p.U##p##TXIF	= 0;*/
				IECbitsU2TX.U2TXIE	= 0;
				break;
				}
			}
		if( U2STAbits.UTXBF != 0 )
			IFSbitsU2TX.U2TXIF	= 0;
		}

	if( MustSwitch )
		CurrentTask	= ReadyTasks[HighestReadyPriority];
	}

#endif  /*  defined __PIC32MK__ || defined __PIC32MZ__ */

#endif	/*	0 */
#endif	/*	SERIAL_PORTS >= 2 */

#if			SERIAL_PORTS >= 3
DECLAREVECTOR(3)
DECLAREHANDLER(3)
#endif	/*	SERIAL_PORTS >= 3 */

#if			SERIAL_PORTS >= 4
DECLAREVECTOR(4)
DECLAREHANDLER(4)
#endif	/*	SERIAL_PORTS >= 4 */

#if			SERIAL_PORTS >= 5
//DECLAREVECTOR(5)
//DECLAREHANDLER(5)
#endif	/*	SERIAL_PORTS >= 5 */

#if			SERIAL_PORTS >= 6
DECLAREVECTOR(6)
DECLAREHANDLER(6)
#endif	/*	SERIAL_PORTS > 6 */
/*============================================================================*/
#endif	/*	defined __XC32__ || defined __C32_VERSION__ */
/*============================================================================*/
