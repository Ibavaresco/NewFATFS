/**
  ******************************************************************************
  *  FatFs - Generic FAT file system module  R0.12c (C)ChaN, 2017
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *****************************************************************************/

/*============================================================================*/
#if			!defined _FFSCONF_H__
#define _FFSCONF_H__
/*============================================================================*/


/*-----------------------------------------------------------------------------
/ Additional user header to be used
/-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/-----------------------------------------------------------------------------*/

#define FF_CODE_PAGE         850
/* This option specifies the OEM code page to be used on the target system.
/  Incorrect setting of the code page can cause a file open failure.
/
/   1   - ASCII (No extended character. Non-LFN cfg. only)
/   437 - U.S.
/   720 - Arabic
/   737 - Greek
/   771 - KBL
/   775 - Baltic
/   850 - Latin 1
/   852 - Latin 2
/   855 - Cyrillic
/   857 - Turkish
/   860 - Portuguese
/   861 - Icelandic
/   862 - Hebrew
/   863 - Canadian French
/   864 - Arabic
/   865 - Nordic
/   866 - Russian
/   869 - Greek 2
/   932 - Japanese (DBCS)
/   936 - Simplified Chinese (DBCS)
/   949 - Korean (DBCS)
/   950 - Traditional Chinese (DBCS)
*/

#define FFS_CONFIG_LFNSUPPORT				  2    /* 0 to 3 */
#define FFS_CONFIG_MAXIMUMLFN				255  /* Maximum LFN length to handle (12 to 255) */

/* The FFS_CONFIG_LFNSUPPORT switches the support of long file name (LFN).
/
/   0: Disable support of LFN. FFS_CONFIG_MAXIMUMLFN has no effect.
/   1: Enable LFN with static working buffer on the BSS. Always NOT thread-safe.
/   2: Enable LFN with dynamic working buffer on the STACK.
/   3: Enable LFN with dynamic working buffer on the HEAP.
/
/  To enable the LFN, Unicode handling functions (option/unicode.c) must be added
/  to the project. The working buffer occupies (FFS_CONFIG_MAXIMUMLFN + 1) * 2 bytes and
/  additional 608 bytes at exFAT enabled. FFS_CONFIG_MAXIMUMLFN can be in range from 12 to 255.
/  It should be set 255 to support full featured LFN operations.
/  When use stack for the working buffer, take care on stack overflow. When use heap
/  memory for the working buffer, memory management functions, FFS_malloc() and
/  FFS_free(), must be added to the project. */

#define FFS_CONFIG_LFNUNICODE				  0 /* 0:ANSI/OEM or 1:Unicode */
/* This option switches character encoding on the API. (0:ANSI/OEM or 1:UTF-16)
/  To use Unicode string for the path name, enable LFN and set _LFN_UNICODE = 1.
/  This option also affects behavior of string I/O functions. */

/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/----------------------------------------------------------------------------*/

/* Maximum mumber of physical drives that can be attached. */
#define FFS_CONFIG_MAXDRIVES				  2
/* Maximum number of volumes (logical drives) that can be mounted. */
#define FFS_CONFIG_MAXVOLUMES   			  2

#define FFS_CONFIG_MULTIPARTITION			  1 /* 0:Single partition, 1:Multiple partition */
/* This option switches support of multi-partition on a physical drive.
/  By default (0), each logical drive number is bound to the same physical drive
/  number and only an FAT volume found on the physical drive will be mounted.
/  When multi-partition is enabled (1), each logical drive number can be bound to
/  arbitrary physical drive and partition. Also f_fdisk()
/  function will be available. */
#define FFS_CONFIG_MINIMUMSECTORSIZE		512  /* 512, 1024, 2048 or 4096 */
#define FFS_CONFIG_MAXIMUMSECTORSIZE		512  /* 512, 1024, 2048 or 4096 */
/* These options configure the range of sector size to be supported. (512, 1024,
/  2048 or 4096) Always set both 512 for most systems, all type of memory cards and
/  harddisk. But a larger value may be required for on-board flash memory and some
/  type of optical media. When FFS_CONFIG_MAXIMUMSECTORSIZE is larger than FFS_CONFIG_MINIMUMSECTORSIZE, FatFs is configured
/  to variable sector size and GET_SECTOR_SIZE command must be implemented to the
/  disk_ioctl() function. */

#define FFS_CONFIG_USELBA64					  0
/* This option switches support for 64-bit LBA. (0:Disable or 1:Enable)
/  To enable the 64-bit LBA, also exFAT needs to be enabled. */

#define FFS_CONFIG_MINGPT					0x10000000


#define	FFS_CONFIG_USETRIM					  0
/* This option switches support of ATA-TRIM. (0:Disable or 1:Enable)
/  To enable Trim function, also CTRL_TRIM command should be implemented to the
/  disk_ioctl() function. */

#define FFS_CONFIG_NOFSINFO    				  0 /* 0,1,2 or 3 */
/* If you need to know correct free space on the FAT32 volume, set bit 0 of this
/  option, and f_getfree() function at first time after volume mount will force
/  a full FAT scan. Bit 1 controls the use of last allocated cluster number.
/
/  bit0=0: Use free cluster count in the FSINFO if available.
/  bit0=1: Do not trust free cluster count in the FSINFO.
/  bit1=0: Use last allocated cluster number in the FSINFO if available.
/  bit1=1: Do not trust last allocated cluster number in the FSINFO.
*/

/*---------------------------------------------------------------------------/
/ System Configurations
/----------------------------------------------------------------------------*/

#define FFS_CONFIG_MAXIMUMOPENFILES			  5
#define FFS_CONFIG_MAXOBJECTS				  5     /* 0:Disable or >=1:Enable */
/* The option FFS_CONFIG_MAXOBJECTS switches file lock function to control duplicated file open
/  and illegal operation to open objects. This option must be 0 when _FS_READONLY
/  is 1.
/
/  0:  Disable file lock function. To avoid volume corruption, application program
/      should avoid illegal open, remove and rename to the open objects.
/  >0: Enable file lock function. The value defines how many files/sub-directories
/      can be opened simultaneously under file lock control. Note that the file
/      lock control is independent of re-entrancy. */

#define FFS_CONFIG_REENTRANT				  1  /* 0:Disable or 1:Enable */

#define FFS_CONFIG_TIMEOUT					 20 /* Timeout period in unit of time ticks */
/* The option FFS_CONFIG_REENTRANT switches the re-entrancy (thread safe) of the FatFs
/  module itself. Note that regardless of this option, file access to different
/  volume is always re-entrant and volume control functions, mount(), f_mkfs()
/  and f_fdisk() function, are always not re-entrant. Only file/directory access
/  to the same volume is under control of this function.
/
/   0: Disable re-entrancy. FFS_CONFIG_TIMEOUT and ffs_sync_t have no effect.
/   1: Enable re-entrancy. Also user provided synchronization handlers,
/      FFS_TakeMutex(), FFS_ReleaseMutex(), FFS_DeleteMutex() and FFS_CreateMutex()
/      function, must be added to the project. Samples are available in
/      option/syscall.c.
/
/  The FFS_CONFIG_TIMEOUT defines timeout period in unit of time tick.
/  The ffs_sync_t defines O/S dependent sync object type. e.g. HANDLE, ID, OS_EVENT*,
/  SemaphoreHandle_t and etc.. A header file for O/S definitions needs to be
/  included somewhere in the scope of ffs.h. */

/*============================================================================*/
#endif	/*	!defined _FFSCONF_H__ */
/*============================================================================*/
