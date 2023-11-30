/*============================================================================*/
#if			!defined __FFS_SYSCALLS_H__
#define __FFS_SYSCALLS_H__
/*============================================================================*/
#include <stdint.h>
#include <stdlib.h>
/*============================================================================*/
typedef void *ffs_sync_t;
/*============================================================================*/
#define DeclareIntState(s) int s
/*============================================================================*/
int			FFS_EnterCriticalSection	( void );
void		FFS_ExitCriticalSection     ( int s );
ffs_sync_t	FFS_CreateMutex				( void );
void		FFS_DeleteMutex				( ffs_sync_t sobj );
int			FFS_TakeMutex				( ffs_sync_t sobj );
int			FFS_ReleaseMutex			( ffs_sync_t sobj );
void		*FFS_malloc					( size_t size );
void		FFS_free					( void *ptr );
/*============================================================================*/
#endif	/*	!defined __FFS_SYSCALLS_H__ */
/*============================================================================*/
