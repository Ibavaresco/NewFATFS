/*============================================================================*/

#include <stdlib.h>
#include <usart.h>
#include <uart.h>
#include <sysclk.h>
#include <sam_gpio/sam_gpio.h>
#include <pio.h>

/*============================================================================*/

/* Scheduler include files. */
#include "Portable.h"
#include "Queue.h"
#include "PortSAM3Internals.h"
#include "PortableInternals.h"
#include "SimpleRTOSInternals.h"
#include "SerialSAM3.h"

/*============================================================================*/

#define	SERIAL_PORTS	 4
#define QUEUE_LENGTH	32

/*============================================================================*/

/* The queues used to communicate between tasks and ISR's. */
/*static*/ queue_t			QueuesRX[SERIAL_PORTS]; 
/*static*/ unsigned char	BuffersRX[SERIAL_PORTS][QUEUE_LENGTH]; 
/*static*/ queue_t			QueuesTX[SERIAL_PORTS]; 
/*static*/ unsigned char	BuffersTX[SERIAL_PORTS][QUEUE_LENGTH]; 

/*============================================================================*/
extern __attribute((weak)) void UserConfigUART1( void );
extern __attribute((weak)) void UserConfigUART2( void );
extern __attribute((weak)) void UserConfigUART3( void );
extern __attribute((weak)) void UserConfigUART4( void );
extern __attribute((weak)) void UserConfigUART5( void );
extern __attribute((weak)) void UserConfigUART6( void );
/*============================================================================*/
static const IRQn_Type	IRQn[]		= { UART_IRQn, USART0_IRQn, USART1_IRQn, USART2_IRQn };
static const int		IDs[]		= { ID_UART, ID_USART0, ID_USART1, ID_USART2 };

int SerialPortInitMinimal( unsigned int port, unsigned long BaudRate )
	{
	sam_usart_opt_t	usart_settings =
		{
		.baudrate		= BaudRate,
		.char_length	= US_MR_CHRL_8_BIT,
		.parity_type	= US_MR_PAR_NO,
		.stop_bits		= US_MR_NBSTOP_1_BIT,
		.channel_mode	= US_MR_CHMODE_NORMAL,
		.irda_filter	= 0	/* This field is only used in IrDA mode. */
		};

	if( port < 1 || port > SERIAL_PORTS )
		return 0;

	/* Create the queues used by the com test task. */
	QueueInit( &QueuesRX[port-1], sizeof( signed char ), QUEUE_LENGTH, &BuffersRX[port-1], QUEUE_SWITCH_IN_ISR );
	QueueInit( &QueuesTX[port-1], sizeof( signed char ), QUEUE_LENGTH, &BuffersTX[port-1], QUEUE_SWITCH_NORMAL );

	/* Enable the peripheral clock in the PMC. */
	sysclk_enable_peripheral_clock( IDs[port-1] );

	switch( port )
		{
		case 1:
			{
			sam_uart_opt_t	uart_settings =
				{
				/** MCK for UART */
				.ul_mck			= sysclk_get_cpu_hz(),
				/** Expected baud rate */
				.ul_baudrate	= BaudRate,
				/** Initialize value for UART mode register */
				.ul_mode		= UART_MR_PAR_NO | UART_MR_CHMODE_NORMAL
				};

			gpio_configure_pin( PIO_PA8_IDX, PIO_TYPE_PIO_PERIPH_A );
			gpio_configure_pin( PIO_PA9_IDX, PIO_TYPE_PIO_PERIPH_A );

			uart_init( UART, &uart_settings );

			/* Enable the receiver and transmitter. */
			uart_enable_tx( UART );
			uart_enable_rx( UART );

			/* Configure and enable interrupt of USART. */
			irq_register_handler( IRQn[port-1], MAX_SYSCALL_INTERRUPT_PRIORITY );
			NVIC_EnableIRQ( IRQn[port-1] );
			uart_enable_interrupt( UART, US_IER_RXRDY );

            if( UserConfigUART1 != NULL )
                UserConfigUART1();
			break;
			}
#if			SERIAL_PORTS >= 2
		case 2:
			gpio_configure_pin( PIO_PA10_IDX, PIO_TYPE_PIO_PERIPH_A );
			gpio_configure_pin( PIO_PA11_IDX, PIO_TYPE_PIO_PERIPH_A );

			/* Configure the USART. */
			usart_init_rs232( USART0, &usart_settings, sysclk_get_cpu_hz() );

			/* Enable the receiver and transmitter. */
			usart_enable_tx( USART0 );
			usart_enable_rx( USART0 );

			/* Configure and enable interrupt of USART. */
			irq_register_handler( IRQn[port-1], MAX_SYSCALL_INTERRUPT_PRIORITY );
			NVIC_EnableIRQ( IRQn[port-1] );
			usart_enable_interrupt( USART0, US_IER_RXRDY );

            if( UserConfigUART2 != NULL )
                UserConfigUART2();
			break;
#endif	/*	SERIAL_PORTS >= 2 */
#if			SERIAL_PORTS >= 3
		case 3:
			gpio_configure_pin( PIO_PA12_IDX, PIO_TYPE_PIO_PERIPH_A );
			gpio_configure_pin( PIO_PA13_IDX, PIO_TYPE_PIO_PERIPH_A );

			/* Configure the USART. */
			usart_init_rs232( USART1, &usart_settings, sysclk_get_cpu_hz() );

			/* Enable the receiver and transmitter. */
			usart_enable_tx( USART1 );
			usart_enable_rx( USART1 );

			/* Configure and enable interrupt of USART. */
			irq_register_handler( IRQn[port-1], MAX_SYSCALL_INTERRUPT_PRIORITY );
			NVIC_EnableIRQ( IRQn[port-1] );
			usart_enable_interrupt( USART1, US_IER_RXRDY );

            if( UserConfigUART3 != NULL )
                UserConfigUART3();
			break;
#endif	/*	SERIAL_PORTS >= 3 */
#if			SERIAL_PORTS >= 4
		case 4:
			gpio_configure_pin( PIO_PB21_IDX, PIO_TYPE_PIO_PERIPH_A );
			gpio_configure_pin( PIO_PB20_IDX, PIO_TYPE_PIO_PERIPH_A );

			/* Configure the USART. */
			usart_init_rs232( USART2, &usart_settings, sysclk_get_cpu_hz() );

			/* Enable the receiver and transmitter. */
			usart_enable_tx( USART2 );
			usart_enable_rx( USART2 );

			/* Configure and enable interrupt of USART. */
			irq_register_handler( IRQn[port-1], MAX_SYSCALL_INTERRUPT_PRIORITY );
			NVIC_EnableIRQ( IRQn[port-1] );
			usart_enable_interrupt( USART2, US_IER_RXRDY );

            if( UserConfigUART4 != NULL )
                UserConfigUART4();
			break;
#endif	/*	SERIAL_PORTS >= 4 */
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
			uart_enable_interrupt( UART, US_IER_TXRDY );
#if			SERIAL_PORTS >= 2
		else if( port == 2 )
			usart_enable_interrupt( USART0, US_IER_TXRDY );
#endif	/*	SERIAL_PORTS >= 2 */
#if			SERIAL_PORTS >= 3
		else if( port == 3 )
			usart_enable_interrupt( USART1, US_IER_TXRDY );
#endif	/*	SERIAL_PORTS >= 3 */
#if			SERIAL_PORTS >= 4
		else if( port == 4 )
			usart_enable_interrupt( USART2, US_IER_TXRDY );
#endif	/*	SERIAL_PORTS >= 4 */


		return 1;
		}
	return 0;
	}

/*============================================================================*/
static void __attribute__((noinline)) UART_ISR( void )
	{
	unsigned long	Status;
	uint8_t		Aux;
	int				MustSwitch	= 0;
	static char		c;

	Status	= uart_get_status( UART ) & uart_get_interrupt_mask( UART );

	if( Status & US_CSR_RXRDY )
		{
		while( uart_get_status( UART ) & US_CSR_RXRDY )
			{
			/* Retrieve the received character and place it in the queue of	
			received characters. */
			uart_read( UART, &Aux );
			c	= (unsigned char)Aux;

			if( QueueWriteFromISR( &QueuesRX[0], &c ) > 1 )
				MustSwitch	= 1;
			}
		}

    /* Are any Tx interrupts pending? */
	if( Status & US_CSR_TXRDY )
		{
		while( uart_get_status( UART ) & US_CSR_TXRDY )
			{
			int	Result;
			Result = QueueReadFromISR( &QueuesTX[0], &c );
			if( Result > 0 )
				{
				/* Send the next character queued for Tx. */
				uart_write( UART, c );
				if( Result > 1 )
					MustSwitch	= 1;
				}
			else
				{
				/* Queue empty, nothing to send. */
				uart_disable_interrupt( UART, US_IER_TXRDY );
				break;
				}
			}
		}

	if( MustSwitch )
		CurrentTask	= ReadyTasks[HighestReadyPriority];
	}
/*============================================================================*/
void __attribute__((naked)) UART_Handler( void )
	{
	SAVE_CONTEXT();

	UART_ISR();

	RESTORE_CONTEXT();
	}
	
/*============================================================================*/

#if			SERIAL_PORTS >= 2

static void __attribute__((noinline)) USART_ISR( unsigned int usart )
	{
	static Usart * const Usarts[]	= { USART0, USART1, USART2 };
	Usart			*u;
	unsigned long	Status;
	uint32_t		Aux;
	int				MustSwitch	= 0;
	static char		c;

	if( usart >= sizeof Usarts / sizeof Usarts[0] )
		return;

	u	= Usarts[usart];

	Status	= usart_get_status( u ) & usart_get_interrupt_mask( u );

	if( Status & US_CSR_RXRDY )
		{
		while( usart_get_status( u ) & US_CSR_RXRDY )
			{
			/* Retrieve the received character and place it in the queue of	
			received characters. */
			usart_read( u, &Aux );
			c	= (unsigned char)Aux;

			if( QueueWriteFromISR( &QueuesRX[usart+1], &c ) > 1 )
				MustSwitch	= 1;
			}
		}

    /* Are any Tx interrupts pending? */
	if( Status & US_CSR_TXRDY )
		{
		while( usart_get_status( u ) & US_CSR_TXRDY )
			{
			int	Result;
			Result = QueueReadFromISR( &QueuesTX[usart+1], &c );
			if( Result > 0 )
				{
				/* Send the next character queued for Tx. */
				usart_write( u, c );
				if( Result > 1 )
					MustSwitch	= 1;
				}
			else
				{
				/* Queue empty, nothing to send. */
				usart_disable_interrupt( u, US_IER_TXRDY );
				break;
				}
			}
		}

	if( MustSwitch )
		CurrentTask	= ReadyTasks[HighestReadyPriority];
	}

/*============================================================================*/
void __attribute__((naked)) USART0_Handler( void )
	{
	SAVE_CONTEXT();

	USART_ISR( 0 );

	RESTORE_CONTEXT();
	}

#endif	/*	SERIAL_PORTS >= 2 */

/*============================================================================*/

#if			SERIAL_PORTS >= 3

void __attribute__((naked)) USART1_Handler( void )
	{
	SAVE_CONTEXT();

	USART_ISR( 1 );

	RESTORE_CONTEXT();
	}
	
#endif	/*	SERIAL_PORTS >= 3 */

/*============================================================================*/

#if			SERIAL_PORTS >= 4

void __attribute__((naked)) USART2_Handler( void )
	{
	SAVE_CONTEXT();

	USART_ISR( 2 );

	RESTORE_CONTEXT();
	}

#endif	/*	SERIAL_PORTS >= 4 */

/*============================================================================*/
