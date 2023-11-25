/*============================================================================*/
#if			!defined __SERIALPIC32_H__
#define __SERIALPIC32_H__
/*============================================================================*/
#include "SimpleRTOS.h"
/*============================================================================*/
int	SerialPortInitMinimal	( unsigned int port, unsigned long BaudRate );
int	SerialReceive			( unsigned int port, tickcount_t TimeToWait );
int	SerialTransmit			( unsigned int port, signed char c, tickcount_t TimeToWait );
/*============================================================================*/
#endif	/*	!defined __SERIALPIC32_H__ */
/*============================================================================*/
