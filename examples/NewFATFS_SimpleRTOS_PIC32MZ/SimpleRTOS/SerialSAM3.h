/*============================================================================*/
#if			!defined __SERIALSAM3_H__
#define __SERIALSAM3_H__
/*============================================================================*/
#include "SimpleRTOS.h"
/*============================================================================*/

typedef enum
	{ 
	COM_UART	= 1,	SERIAL1	= COM_UART,
	COM_USART0,			SERIAL2	= COM_USART0,
	COM_USART1,			SERIAL3	= COM_USART1,
	COM_USART2,			SERIAL4	= COM_USART2,
	} comport_t;

/*============================================================================*/

int	SerialPortInitMinimal	( unsigned int port, unsigned long BaudRate );
int	SerialReceive			( unsigned int port, tickcount_t TimeToWait );
int	SerialTransmit			( unsigned int port, signed char c, tickcount_t TimeToWait );
int	SerialWaitForTXCompletion	( unsigned int port, tickcount_t TimeToWait );

/*============================================================================*/
#endif	/*	!defined __SERIALSAM3_H__ */
/*============================================================================*/
