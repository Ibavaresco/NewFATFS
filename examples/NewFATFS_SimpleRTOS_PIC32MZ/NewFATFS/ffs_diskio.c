/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2017        */
/*                                                                       */
/*   Portions COPYRIGHT 2017 STMicroelectronics                          */
/*   Portions Copyright (C) 2017, ChaN, all right reserved               */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various existing      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
/*============================================================================*/
/* Includes ------------------------------------------------------------------*/
/*============================================================================*/
#include <string.h>
#include "ffs_diskio.h"
/*============================================================================*/
#if defined ( __GNUC__ )
#ifndef __weak
#define __weak __attribute__((weak))
#endif
#endif
/*============================================================================*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*============================================================================*/
typedef struct
	{
	const ffs_diskio_t	*Driver;
	void				*Cookie;
	int					Initialized;
	int					Next;
	char				Name[4];
	} diskdrive_t;
/*============================================================================*/
typedef struct
	{
	diskdrive_t			Drives[FFS_CONFIG_MAXDRIVES];
	int					Initialized;
	int					IndexOfFreeEntry;
	volatile int		Count;
	} physicaldrives_t;
/*============================================================================*/
physicaldrives_t FFS_PhysicalDrives;
/*============================================================================*/
static void InitializePhysicalDrives( void )
	{
	int	i;

	if( FFS_PhysicalDrives.Initialized )
		return;

	for( i = 0; i < sizeof FFS_PhysicalDrives.Drives / sizeof FFS_PhysicalDrives.Drives[0]; i++ )
		{
		FFS_PhysicalDrives.Drives[i].Driver			= NULL;
		FFS_PhysicalDrives.Drives[i].Cookie			= NULL;
		FFS_PhysicalDrives.Drives[i].Next			= i + 1;
		FFS_PhysicalDrives.Drives[i].Initialized	=  0;
		}
	FFS_PhysicalDrives.Drives[i-1].Next	= -1;

	FFS_PhysicalDrives.IndexOfFreeEntry	=  0;
	FFS_PhysicalDrives.Count			=  0;
	FFS_PhysicalDrives.Initialized		=  1;
	}
/*============================================================================*/
int FFS_GetPhysicalDriveName( int disknum, char *pdname )
	{
	memset( pdname, 0, sizeof FFS_PhysicalDrives.Drives[disknum].Name + 1 );

	if( disknum < 0 || disknum >= sizeof FFS_PhysicalDrives.Drives / sizeof FFS_PhysicalDrives.Drives[0] )
		return -1;

	if( FFS_PhysicalDrives.Drives[disknum].Name[0] == '\0' )
		return 0;

	strlcpy( pdname, FFS_PhysicalDrives.Drives[disknum].Name, sizeof FFS_PhysicalDrives.Drives[disknum].Name + 1 );

	return 1;
	}
/*============================================================================*/
int FFS_FindPhysicalDriveByName( const char *pdname )
	{
	/* The field 'Drives[].Name' does not have space for the string terminator, we must provide space for it. */
	char	s[sizeof FFS_PhysicalDrives.Drives[0].Name + 1];
	int		i;

	i	= strlen( pdname );

	/* The last character in 'pdname' is a colon... */
	if( i > 0 && pdname[i-1] == ':' )
		/* ...we must ignore it. */
		i--;

	/* The name is either empty or too long... */
	if( i < 1 || i >= sizeof s )
		/* ...it won't be found anyway and we must not return 'not found' in this case. */
		return FFS_FINDPHYSICALDRIVE_INVALIDNAME;

	/* We must copy the name to a local variable so we can get rid of the colon, if it exists. */
	strlcpy( s, pdname, i + 1 );

	/* Look in all entries of 'FFS_PhysicalDrives.Drives' for the drive name. */
	for( i = 0; i < sizeof FFS_PhysicalDrives.Drives / sizeof FFS_PhysicalDrives.Drives[0]; i++ )
		/* We must use 'strncmp' because 'Drives[i].Name' may not have a '\0' at its end. */
		if( strncmp( s, FFS_PhysicalDrives.Drives[i].Name, sizeof FFS_PhysicalDrives.Drives[i].Name ) == 0 )
			/* We have found the drive name we are looking for. */
			return i;

	/* We have iterated through all entries and did not find the name. */
	return FFS_FINDPHYSICALDRIVE_NOTFOUND;
	}
/*============================================================================*/
/**
  * @brief  Links a compatible diskio driver/lun id and increments the number of active
  *         linked drivers.
  * @note   The number of linked drivers (volumes) is up to 10 due to FatFs limits.
  * @param  Driver: pointer to the disk IO Driver structure
  * @param  path: pointer to the logical drive path
  * @param  lun : only used for USB Key Disk to add multi-lun management
            else the parameter must be equal to 0
  * @retval Returns 0 in case of success, otherwise 1.
  */
/*============================================================================*/
int FFS_AttachDrive( const char *pdname, void *cookie, const ffs_diskio_t *driver )
	{
	diskdrive_t	*Drive;
	int			disknum;
	int			i;
	char		s[6];

	if( !FFS_PhysicalDrives.Initialized )
		InitializePhysicalDrives();

	// There is no room to load more drivers...
	if( FFS_PhysicalDrives.IndexOfFreeEntry < 0 || FFS_PhysicalDrives.IndexOfFreeEntry >= sizeof FFS_PhysicalDrives.Drives / sizeof FFS_PhysicalDrives.Drives[0] )
		// ... return a failure status;
		return -1;

	// Either the name is invalid or a drive with this name is already loaded...
	if( FFS_FindPhysicalDriveByName( pdname ) != FFS_FINDPHYSICALDRIVE_NOTFOUND )
		// ... return a failure status;
		return -1;

	strlcpy( s, pdname, sizeof s );
	i	= strlen( s );
	if( s[i-1] == ':' )
		s[i-1]	= '\0';

	disknum								= FFS_PhysicalDrives.IndexOfFreeEntry;
	Drive								= &FFS_PhysicalDrives.Drives[disknum];
	FFS_PhysicalDrives.IndexOfFreeEntry	= Drive->Next;

	Drive->Driver						= driver;
	Drive->Cookie						= cookie;
	memcpy( Drive->Name, s, sizeof Drive->Name );
	Drive->Next							= -1;
	Drive->Initialized					=  0;

	FFS_PhysicalDrives.Count++;

	return 0;
	}
/*============================================================================*/
/**
  * @brief  Unlinks a diskio driver and decrements the number of active linked
  *         drivers.
  * @param  path: pointer to the logical drive path
  * @param  lun : not used
  * @retval Returns 0 in case of success, otherwise 1.
  */
/*============================================================================*/
int FFS_DetachDrive( const char *pdname )
	{
	diskdrive_t	*Drive;
	int			disknum;

	// Either the name is invalid or there is no drive with this name loaded...
	if(( disknum = FFS_FindPhysicalDriveByName( pdname )) < 0 )
		// ... return a failure status;
		return -1;

	Drive	= &FFS_PhysicalDrives.Drives[disknum];

	if( Drive->Driver != NULL && Drive->Driver->disk_uninitialize != NULL )
		Drive->Driver->disk_uninitialize( Drive->Cookie, Drive->Driver );

	memset( Drive, 0, sizeof *Drive );

	Drive->Next							= FFS_PhysicalDrives.IndexOfFreeEntry;
	FFS_PhysicalDrives.IndexOfFreeEntry	= disknum;

	FFS_PhysicalDrives.Count--;

	return 0;
	}
/*============================================================================*/
/**
  * @brief  Gets number of linked drivers to the FatFs module.
  * @param  None
  * @retval Number of attached drivers.
  */
/*============================================================================*/
int FFS_GetAttachedDriversCount( void )
	{
	return FFS_PhysicalDrives.Count;
	}
/*============================================================================*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*============================================================================*/
/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval ffs_diskstatus_t: Operation status
  */
/*============================================================================*/
ffs_diskstatus_t disk_status( int pdrv )	/* Physical drive number to identify the drive */
	{
	diskdrive_t			*Drive;

	if( pdrv < 0 || pdrv >= sizeof FFS_PhysicalDrives.Drives / sizeof FFS_PhysicalDrives.Drives[0] )
		return RES_ERROR;

	Drive	= &FFS_PhysicalDrives.Drives[pdrv];
	
	if( Drive->Driver == NULL || Drive->Driver->disk_status == NULL )
		return RES_ERROR;

	return Drive->Driver->disk_status( Drive->Cookie );
	}
/*============================================================================*/
/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval ffs_diskstatus_t: Operation status
  */
/*============================================================================*/
ffs_diskstatus_t disk_initialize( int pdrv )				/* Physical drive number to identify the drive */
	{
	ffs_diskstatus_t	Status;
	diskdrive_t			*Drive;

	if( pdrv < 0 || pdrv >= sizeof FFS_PhysicalDrives.Drives / sizeof FFS_PhysicalDrives.Drives[0] )
		return RES_ERROR;

	Drive	= &FFS_PhysicalDrives.Drives[pdrv];
	
	if( Drive->Driver == NULL || Drive->Driver->disk_initialize == NULL )
		return RES_ERROR;

	if( Drive->Initialized )
		return STA_OK;

	if((( Status = Drive->Driver->disk_initialize( Drive->Cookie )) & ~STA_PROTECT ) == STA_OK )
		Drive->Initialized	= 1;

	return Status;
	}
/*============================================================================*/
ffs_diskstatus_t disk_uninitialize( int pdrv )				/* Physical drive number to identify the drive */
	{
	ffs_diskstatus_t	Status;
	diskdrive_t			*Drive;

	if( pdrv < 0 || pdrv >= sizeof FFS_PhysicalDrives.Drives / sizeof FFS_PhysicalDrives.Drives[0] )
		return RES_ERROR;

	Drive	= &FFS_PhysicalDrives.Drives[pdrv];
	
	if( Drive->Driver == NULL || Drive->Driver->disk_uninitialize == NULL )
		return RES_ERROR;

	if( !Drive->Initialized )
		return RES_OK;

	if(( Status = Drive->Driver->disk_uninitialize( Drive->Cookie, Drive->Driver )) == STA_OK )
		Drive->Initialized	= 0;

	return Status;
	}
/*============================================================================*/
/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval ffs_diskresult_t: Operation result
  */
/*============================================================================*/
ffs_diskresult_t disk_read(
	int				pdrv,		/* Physical drive number to identify the drive */
	uint8_t			*buff,		/* Data buffer to store read data */
	uint32_t		sector,	    /* Sector address in LBA */
	unsigned int	count		/* Number of sectors to read */
)
	{
	diskdrive_t	*Drive;

	if( pdrv < 0 || pdrv >= sizeof FFS_PhysicalDrives.Drives / sizeof FFS_PhysicalDrives.Drives[0] )
		return RES_ERROR;

	Drive	= &FFS_PhysicalDrives.Drives[pdrv];
	
	if( Drive->Driver == NULL || Drive->Driver->disk_read == NULL )
		return RES_ERROR;

	return Drive->Driver->disk_read( Drive->Cookie, buff, sector, count );
	}
/*============================================================================*/
/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval ffs_diskresult_t: Operation result
  */
/*============================================================================*/
#if			defined _USE_WRITE && _USE_WRITE == 1
/*============================================================================*/
ffs_diskresult_t disk_write(
	int				pdrv,		/* Physical drive nmuber to identify the drive */
	const uint8_t	*buff,	/* Data to be written */
	uint32_t		sector,		/* Sector address in LBA */
	unsigned int	count        	/* Number of sectors to write */
)
	{
	diskdrive_t	*Drive;

	if( pdrv < 0 || pdrv >= sizeof FFS_PhysicalDrives.Drives / sizeof FFS_PhysicalDrives.Drives[0] )
		return RES_ERROR;

	Drive	= &FFS_PhysicalDrives.Drives[pdrv];
	
	if( Drive->Driver == NULL || Drive->Driver->disk_write == NULL )
		return RES_ERROR;

	return Drive->Driver->disk_write( Drive->Cookie, buff, sector, count );
	}
/*============================================================================*/
#endif	/*	defined _USE_WRITE && _USE_WRITE == 1 */
/*============================================================================*/
/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval ffs_diskresult_t: Operation result
  */
/*============================================================================*/
#if			defined _USE_IOCTL && _USE_IOCTL == 1
/*============================================================================*/
ffs_diskresult_t disk_ioctl( int pdrv, int cmd, void *buff ) /* Physical drive nmuber (0..) */ /* Control code */ /* Buffer to send/receive control data */
	{
	diskdrive_t	*Drive;

	if( pdrv < 0 || pdrv >= sizeof FFS_PhysicalDrives.Drives / sizeof FFS_PhysicalDrives.Drives[0] )
		return RES_ERROR;

	Drive	= &FFS_PhysicalDrives.Drives[pdrv];
	
	if( Drive->Driver == NULL || Drive->Driver->disk_ioctl == NULL )
		return RES_ERROR;

	return Drive->Driver->disk_ioctl( Drive->Cookie, cmd, buff );
	}
/*============================================================================*/
#endif	/*	defined _USE_IOCTL && _USE_IOCTL == 1 */
/*============================================================================*/
/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in uint32_t
  */
/*============================================================================*/
#define	FF_NORTC_YEAR 2023
#define	FF_NORTC_MON	11
#define	FF_NORTC_MDAY	16
/*============================================================================*/
uint32_t __attribute__((weak)) get_fattime( void )
	{
	return ((uint32_t)(FF_NORTC_YEAR - 1980) << 25 | (uint32_t)FF_NORTC_MON << 21 | (uint32_t)FF_NORTC_MDAY << 16);
	}
/*============================================================================*/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
/*============================================================================*/
