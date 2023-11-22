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
#include "PortPIC32Internals.h"
#include <SimpleRTOS.h>
#include <Mutex.h>
/*============================================================================*/
intsave_t FFS_EnterCriticalSection( void )
	{
    return SaveAndDisableInterrupts();
	}
/*============================================================================*/
void FFS_ExitCriticalSection( intsave_t s )
	{
    RestoreInterrupts( s );
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
static mutex_t SystemMutex  =
    {
	.Owner       = NULL,
	.WaitingList = NULL,
	.Count       = 0,
	.Mode        = MUTEX_SWITCH_IMMEDIATE
    };
/*============================================================================*/
ffs_sync_t FFS_CreateMutex( void )
	{
	return &SystemMutex;
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
    MutexTake( sobj, -1 );
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
    MutexGive( sobj, 0 );
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
