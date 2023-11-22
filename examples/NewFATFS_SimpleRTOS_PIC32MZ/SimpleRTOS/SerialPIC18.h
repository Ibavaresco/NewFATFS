/*============================================================================*/
#if			!defined __SERIALPIC18_H__
#define __SERIALPIC18_H__
/*============================================================================*/
#include "SimpleRTOS.h"
/*============================================================================*/
typedef enum
	{ 
	serCOM1, 
	serCOM2, 
	serCOM3, 
	serCOM4, 
	serCOM5, 
	serCOM6, 
	serCOM7, 
	serCOM8 
	} eCOMPort;

typedef enum 
	{ 
	serNO_PARITY, 
	serODD_PARITY, 
	serEVEN_PARITY, 
	serMARK_PARITY, 
	serSPACE_PARITY 
	} eParity;

typedef enum 
	{ 
	serSTOP_1, 
	serSTOP_2 
	} eStopBits;

typedef enum 
	{ 
	serBITS_5, 
	serBITS_6, 
	serBITS_7, 
	serBITS_8 
	} eDataBits;

typedef enum 
	{ 
	ser50,		
	ser75,		
	ser110,		
	ser134,		
	ser150,    
	ser200,
	ser300,		
	ser600,		
	ser1200,	
	ser1800,	
	ser2400,   
	ser4800,
	ser9600,		
	ser19200,	
	ser38400,	
	ser57600,	
	ser115200
	} eBaud;
/*============================================================================*/
int	SerialPortInitMinimal	( unsigned int port, unsigned long BaudRate );
int	SerialReceive			( unsigned int port, tickcount_t TimeToWait );
int	SerialTransmit			( unsigned int port, signed char c, tickcount_t TimeToWait );
/*============================================================================*/
#endif	/*	!defined __SERIALPIC18_H__ */
/*============================================================================*/
