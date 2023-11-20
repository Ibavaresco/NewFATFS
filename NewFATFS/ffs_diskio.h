/*============================================================================*/
/* Low level disk interface module include file   (C)ChaN, 2014               */
/*============================================================================*/
#if			!defined __FFS_DISKIO_H__
#define __FFS_DISKIO_H__
/*============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*/
#include <stdint.h>
//#include <ffsconf.h>
#include "../../../FATFS/Target/ffsconf.h"
/*============================================================================*/
#define _USE_WRITE	1	/* 1: Enable disk_write function */
#define _USE_IOCTL	1	/* 1: Enable disk_ioctl function */
/*============================================================================*/
/* Results of Disk Functions */
/*============================================================================*/
typedef enum
	{
	RES_OK			= 0,	/* 0: Successful */
	RES_ERROR,				/* 1: R/W Error */
	RES_NOTRDY,				/* 2: Not Ready */
	RES_PARERR,				/* 3: Invalid Parameter */
	RES_WRPRT				/* 4: Write Protected */
	} ffs_diskresult_t;
/*============================================================================*/
/* Disk Status Bits (ffs_diskstatus_t) */
/*============================================================================*/
typedef enum
	{
	STA_OK			= 0x00,
	STA_NOINIT		= 0x01,	/* Drive not initialized */
	STA_NODISK		= 0x02,	/* No medium in the drive */
	STA_PROTECT		= 0x04	/* Write protected */
	} ffs_diskstatus_t;
/*============================================================================*/
typedef struct ffs_diskio_tag
	{
	ffs_diskstatus_t (*disk_initialize)		( void *Cookie );											/*!< Initialize Disk Drive                     */
	ffs_diskstatus_t (*disk_uninitialize)	( void *Cookie, const struct ffs_diskio_tag* );				/*!< Initialize Disk Drive                     */
	ffs_diskstatus_t (*disk_status)			( void *Cookie );											/*!< Get Disk Status                           */
	ffs_diskresult_t (*disk_read)			( void *Cookie, uint8_t*, uint32_t, unsigned int );			/*!< Read Sector(s)                            */
	ffs_diskresult_t (*disk_write)			( void *Cookie, const uint8_t*, uint32_t, unsigned int );	/*!< Write Sector(s) when _USE_WRITE = 0       */
	ffs_diskresult_t (*disk_ioctl)			( void *Cookie, int, void* );								/*!< I/O control operation when _USE_IOCTL = 1 */
	} ffs_diskio_t;
/*============================================================================*/
 /**
   * @brief  Global Disk IO Drivers structure definition
   */
 /*============================================================================*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/*============================================================================*/
int	FFS_AttachDrive				( const char *pdname, void *cookie, const ffs_diskio_t *driver );
int	FFS_DetachDrive				( const char *pdname );
int	FFS_GetAttachedDriversNbr	( void );
int	FFS_GetPhysicalDriveName	( int disknum, char *pdname );
/*============================================================================*/
#define	FFS_FINDPHYSICALDRIVE_INVALIDNAME	-2
#define	FFS_FINDPHYSICALDRIVE_NOTFOUND		-1
/*============================================================================*/
int	FFS_FindPhysicalDriveByName	( const char *pdname );
/*============================================================================*/
/* Prototypes for disk control functions */
/*============================================================================*/
ffs_diskstatus_t	disk_initialize		( int pdrv );
ffs_diskstatus_t	disk_uninitialize	( int pdrv );
ffs_diskstatus_t	disk_status			( int pdrv );
ffs_diskresult_t	disk_read			( int pdrv, uint8_t* buff, uint32_t sector, unsigned int count );
ffs_diskresult_t	disk_write			( int pdrv, const uint8_t* buff, uint32_t sector, unsigned int count );
ffs_diskresult_t	disk_ioctl			( int pdrv, int cmd, void *buff );
uint32_t			get_fattime			( void );
/*============================================================================*/
/* Command code for disk_ioctrl function */
/*============================================================================*/

/* Generic command (Used by FatFs) */
#define CTRL_SYNC			0	/* Complete pending write process (needed at _FS_READONLY == 0) */
#define GET_SECTOR_COUNT	1	/* Get media size (needed at _USE_MKFS == 1) */
#define GET_SECTOR_SIZE		2	/* Get sector size (needed at _MAX_SS != _MIN_SS) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (needed at _USE_MKFS == 1) */
#define CTRL_TRIM			4	/* Inform device that the data on the block of sectors is no longer used (needed at FFS_CONFIG_USETRIM == 1) */

/* Generic command (Not used by FatFs) */
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */
#define CTRL_FORMAT			8	/* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */
/*============================================================================*/
#ifdef __cplusplus
}
#endif
/*============================================================================*/
#endif	/*	!defined __FFS_DISKIO_H__ */
/*============================================================================*/
