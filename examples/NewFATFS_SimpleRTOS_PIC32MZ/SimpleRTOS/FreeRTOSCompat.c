/*============================================================================*/
#include "FreeRTOSCompat.h"
/*============================================================================*/
#if			!defined NULL
#define NULL	0
#endif	/*	!defined NULL */
/*============================================================================*/

SemaphoreHandle_t xSemaphoreCreateMutex( void )
	{
	mutex_t	*Mutex;

	Mutex	= malloc( sizeof( mutex_t ));
	if( Mutex != NULL )
		MutexInit( Mutex, MUTEX_SWITCH_IMMEDIATE );
	return Mutex;
	}

/*============================================================================*/

void vSemaphoreDelete( SemaphoreHandle_t xSemaphore )
	{
	free( xSemaphore );
	}

/*============================================================================*/
