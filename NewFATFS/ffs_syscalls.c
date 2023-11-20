/*----------------------------------------------------------------------------*/
/* Sample code of OS dependent controls for FatFs                             */
/* (C)ChaN, 2014                                                              */
/*   Portions COPYRIGHT 2017 STMicroelectronics                               */
/*   Portions Copyright (C) 2014, ChaN, all right reserved                    */
/*----------------------------------------------------------------------------*/

/**
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
**/


/*============================================================================*/
#include "ffs_syscalls.h"
#include "ffs.h"
#include "FreeRTOS.h"
#include "semphr.h"
/*============================================================================*/
void FFS_EnterCriticalSection( void )
	{
	vPortEnterCritical();
	}
/*============================================================================*/
void FFS_ExitCriticalSection( void )
	{
	vPortExitCritical();
	}
/*============================================================================*/
/*----------------------------------------------------------------------------*/
/* Create a Synchronization Object                                            */
/*----------------------------------------------------------------------------*/
/*============================================================================*/
/* This function is called in mount() function to create a new
/  synchronization object, such as semaphore and mutex. When NULL is returned,
/  the mount() function fails with FR_INT_ERR.
*/
/*============================================================================*/
ffs_sync_t FFS_CreateMutex( void )
	{
	return xSemaphoreCreateRecursiveMutex();
	}
/*============================================================================*/
/*----------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                            */
/*----------------------------------------------------------------------------*/
/*============================================================================*/
/* This function is called in mount() function to delete a synchronization
/  object that created with FFS_CreateMutex() function. When a 0 is returned,
/  the mount() function fails with FR_INT_ERR.
*/
/*============================================================================*/
void FFS_DeleteMutex( ffs_sync_t sobj )
	{
	vSemaphoreDelete( sobj );
	}
/*============================================================================*/
/*----------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                         */
/*----------------------------------------------------------------------------*/
/*============================================================================*/
/* This function is called on entering file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/
/*============================================================================*/
int FFS_TakeMutex( ffs_sync_t sobj )
	{
	return xSemaphoreTakeRecursive( sobj, -1 ) == pdTRUE;
	}
/*============================================================================*/
/*----------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                         */
/*----------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
*/
/*============================================================================*/
int FFS_ReleaseMutex( ffs_sync_t sobj )
	{
	return xSemaphoreGiveRecursive( sobj ) == pdTRUE;
	}
/*============================================================================*/
#if _USE_LFN == 3	/* LFN with a working buffer on the heap                  */
/*============================================================================*/
void *FFS_malloc( size_t size )
	{
	return pvPortMalloc( size );
	}
/*============================================================================*/
void FFS_free( void *ptr )
	{
	vPortFree( ptr );
	}
/*============================================================================*/
#endif
/*============================================================================*/
