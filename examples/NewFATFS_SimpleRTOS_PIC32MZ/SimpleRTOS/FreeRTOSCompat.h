/*============================================================================*/
#if			!defined __FREERTOSCOMPAT_H__
#define __FREERTOSCOMPAT_H__
/*============================================================================*/
#include "Mutex.h"
#include "Queue.h"
/*============================================================================*/

typedef enum
	{
	pdFALSE	= 0,
	pdTRUE
	} bool_t;

typedef tickcount_t	TickType_t;

/*============================================================================*/

typedef mutex_t	*SemaphoreHandle_t;

/*============================================================================*/

SemaphoreHandle_t	xSemaphoreCreateMutex	( void );
#define				xSemaphoreTake(s,t)		MutexTake( s, t )
#define				xSemaphoreGive(s)		MutexGive( s )
void				vSemaphoreDelete		( SemaphoreHandle_t xSemaphore );

/*============================================================================*/
#endif	/*	!defined __FREERTOSCOMPAT_H__ */
/*============================================================================*/
