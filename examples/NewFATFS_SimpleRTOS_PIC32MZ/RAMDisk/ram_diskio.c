/*============================================================================*/
#include <string.h>
#include "ram_diskio.h"
/*============================================================================*/
#define RAMDISK_SECTOR_SIZE		512
#define RAMDISK_SECTOR_COUNT	384
/*============================================================================*/
static ffs_diskstatus_t	RAMDisk_initialize	( void *Cookie );
static ffs_diskstatus_t	RAMDisk_status		( void *Cookie );
static ffs_diskresult_t	RAMDisk_read		( void *Cookie, uint8_t*, uint32_t, unsigned int );
static ffs_diskresult_t	RAMDisk_write		( void *Cookie, const uint8_t*, uint32_t, unsigned int );
static ffs_diskresult_t	RAMDisk_ioctl		( void *Cookie, int, void* );
/*============================================================================*/
const ffs_diskio_t	RAMDisk_Driver =
	{
	.disk_initialize	= RAMDisk_initialize,
	.disk_uninitialize	= NULL,
	.disk_status		= RAMDisk_status,
	.disk_read			= RAMDisk_read,
	.disk_write			= RAMDisk_write,
	.disk_ioctl			= RAMDisk_ioctl
	};
/*============================================================================*/
#if			!defined __DEBUG
static 
#else	/*	!defined __DEBUG */
/* Aligning to a 16-byte boundary just for convenience during debugging. */
__attribute__(( aligned( 16 )))
#endif	/*	!defined __DEBUG */
uint8_t	RAMDisk[RAMDISK_SECTOR_COUNT][RAMDISK_SECTOR_SIZE];
/*============================================================================*/
static ffs_diskstatus_t RAMDisk_initialize( void *Cookie )
	{
	return STA_OK;
	}
/*============================================================================*/
static ffs_diskstatus_t RAMDisk_status( void *Cookie )
	{
	return STA_OK;
	}
/*============================================================================*/
static ffs_diskresult_t RAMDisk_read( void *Cookie, uint8_t *buff, uint32_t sector, unsigned int count )
	{
	if( sector >= RAMDISK_SECTOR_COUNT || count > RAMDISK_SECTOR_COUNT || sector + count > RAMDISK_SECTOR_COUNT )
		return RES_PARERR;

	memcpy( buff, &RAMDisk[sector], sizeof RAMDisk[sector] * count );
	return RES_OK;
	}
/*============================================================================*/
static ffs_diskresult_t RAMDisk_write( void *Cookie, const uint8_t *buff, uint32_t sector, unsigned int count )
	{
	if( sector >= RAMDISK_SECTOR_COUNT || count > RAMDISK_SECTOR_COUNT || sector + count > RAMDISK_SECTOR_COUNT )
		return RES_PARERR;

	memcpy( &RAMDisk[sector], buff, sizeof RAMDisk[sector] * count );
	return RES_OK;
	}
/*============================================================================*/
static ffs_diskresult_t RAMDisk_ioctl( void *Cookie, int cmd, void *buff )
	{
	switch( cmd )
		{
		/* Make sure that no pending write process */
		case CTRL_SYNC :
			return RES_OK;

		/* Get number of sectors on the disk (DWORD) */
		case GET_SECTOR_COUNT :
			*(uint32_t*)buff = RAMDISK_SECTOR_COUNT;
			return RES_OK;

		/* Get R/W sector size (WORD) */
		case GET_SECTOR_SIZE :
			*(uint16_t*)buff = RAMDISK_SECTOR_SIZE;
			return RES_OK;

		/* Get erase block size in unit of sector (DWORD) */
		case GET_BLOCK_SIZE :
			*(uint32_t*)buff	= 1;
			return RES_OK;
		}

	return RES_PARERR;
	}
/*============================================================================*/
