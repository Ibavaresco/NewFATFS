/*============================================================================*/
/*

NewFATFS <https://github.com/Ibavaresco/NewFATFS>

FAT File System routines based on Chan's FatFs (<http://elm-chan.org/fsw/ff/>).
Please see his original copyright and license further below.
--------------------------------------------------------------------------------
Copyright (c)2022-2023, Isaac Marino Bavaresco (isaacbavaresco@yahoo.com.br)

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Neither the name of the author nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------------

Although most of the code is still Chan's, this version has extensive changes
to its structure and API.

The main goal was to implement the most POSIX-compliant version possible, so
we could reuse code from desktop applications without (or with minimal) changes.

Another goal was to integrate seamlessly with NewLib, which I think was
acomplished very well.

The original version has a lot of issues, including security related, but the
most annoying one is its weird API.

In 2005 I wrote a FLASH file system for a commercial product and it proved to
be very good, although only suitable for embedded memories. In 2020 the product's
lifecycle was (long) past, then I published it on GitHub:
(<https://github.com/Ibavaresco/FileSystem/tree/master>)

In 2013 I started development of a new product that needed SD-Card support. The
selected MCU was an Atmel ARM, and the application automatically generated by
ASF included Chan's FatFs. It annoyed me that its API was so weird, so I wrote
a (more or less) POSIX compatibility wrapper layer
(<http://www.piclist.com/techref/member/IMB-yahoo-J86/Atmel-s-FatFS-Wrapper.htm>)

Later I re-wrote it in a NewLib-compatible format. It was published on
<https://spaces.atmel.com/gf/project/posixfatfs>, but now it is not accesible
anymore.

Now it is time to go all the way and create a native version without adaptation
layers, and in the process fix a lot of other issues.

First I "maximized" the configuration. That is, I removed all the options that
reduced features (FF_FS_READONLY, FF_FS_MINIMIZE, FF_FS_TINY, etc.) and enabled
all the options that included features (FF_FS_EXFAT, FF_USE_LFN, etc.).

Then I removed all the conditional compilation directives and replaced several
macros with their values. Also, I replaced several "magic-numbers" spread all
over the code. There are still a lot of them that I plan to replace as time goes
by.

A code re-formatter (Astyle) was used to change the style to Whitesmiths and fix
a lot of unreadable and confusing formatting.

I noticed that the way he uses the semaphores/mutexes was not safe enough and
left some critical parts unprotected from race conditions, so I changed it.
I chose a simple solution of a single mutex for all the filesystem, but that may
not be ideal for some configurations and I plan to add more options in the future.
I think that the ideal solution is comprised of a global mutex to protect the
accesses to filesystem-wide objects for a short time and one mutex for each volume
or physical drive (that will depend on whether the physical drive layer can
support concurrency or not).

Another point that I think he got wrong was the necessity for several LFN buffers.
In my analysis, it seems that the LFN buffers will never be used concurrently,
because they are always initialized only after the filesystem is locked and
released before the filesystem is unlocked. Besides, there is just one pointer in
the 'FATFS' struct. I'm still trying to find some condition that requires several
buffers, but for now I'm using one static buffer in each 'ffs_volume_t' struct.

The locking mechanism is another complicated point. I chose a different approach
of two or threee function layers, to simplify the logic and provide some sort of
"finally" feature.

I also changed most of the types and macros names, to provide a more homogeneous
approach and require less memorization. I plan to change many of the variables and
field names too, because most of them doesn't make much sense to me.

I used a different way of loading physical drivers, based on code from STM, but
with my personal touch. Also, I totally changed the way volumes are mounted and
unmonted, because I never managed the original way to work correctly. If I unmounted
a volume, I could not manage to mount it again without a reboot.

As the new file system is POSIX and NewLib compatible, we can use all the resources
of the C library, including bufferd files and the functions that work with them
(fprintf, fscanf, etc.). That rendered useless several of the FatFs functions
(f_gets, f_puts, f_printf).


WARNING: Although I tested most of the code, it is still under development. I am
publishing it now to allow for others to try it and help in the testing and
perhaps in the development too.

*/
/*============================================================================*/

/*-----------------------------------------------------------------------------/
/  FatFs - Generic FAT Filesystem module  R0.15                                /
/------------------------------------------------------------------------------/
/
/ Copyright (C) 2022, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/-----------------------------------------------------------------------------*/

/*============================================================================*/
#if			!defined _FFS_H__
#define _FFS_H__
/*============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*/
#include <stdint.h>
#include <sys/types.h>
#include "ffsconf.h"		/* FatFs configuration options */
/*============================================================================*/
/* Type of file size and LBA variables */
/*============================================================================*/
#if			defined FFS_CONFIG_USELBA64 && FFS_CONFIG_USELBA64 != 0
typedef uint64_t		filesize_t;
typedef uint64_t		lba_t;
typedef _off64_t		ffs_off_t;
#else	/*	defined FFS_CONFIG_USELBA64 && FFS_CONFIG_USELBA64 != 0 */
typedef uint32_t		filesize_t;
typedef uint32_t		lba_t;
typedef off_t			ffs_off_t;
#endif	/*	defined FFS_CONFIG_USELBA64 && FFS_CONFIG_USELBA64 != 0 */

typedef uint16_t		ffs_wchar_t;
/*============================================================================*/
/* Type of path name strings on FatFs API (ffs_char_t) */
/*============================================================================*/
#if			!defined FFS_CONFIG_LFNUNICODE || FFS_CONFIG_LFNUNICODE == 0
	typedef char		ffs_char_t;
	#define _T(x)		x
	#define _TEXT(x)	x
#elif		FFS_CONFIG_LFNUNICODE == 1 	/* Unicode in UTF-16 encoding */
	typedef ffs_wchar_t	ffs_char_t;
	#define _T(x)		u##x
	#define _TEXT(x)	u##x
#elif		FFS_CONFIG_LFNUNICODE == 2		/* Unicode in UTF-8 encoding */
	typedef char		ffs_char_t;
	#define _T(x)		u8##x
	#define _TEXT(x)	u8##x
#elif		FFS_CONFIG_LFNUNICODE == 3		/* Unicode in UTF-32 encoding */
	typedef uint32_t	ffs_char_t;
	#define _T(x) 		U##x
	#define _TEXT(x) 	U##x
#else	/*	!defined FFS_CONFIG_LFNUNICODE || FFS_CONFIG_LFNUNICODE == 0 */
	#error Wrong FFS_CONFIG_LFNUNICODE setting
#endif	/*	!defined FFS_CONFIG_LFNUNICODE || FFS_CONFIG_LFNUNICODE == 0 */
/*============================================================================*/
/* File attribute bits for directory entry (ffs_fileinfo_t.fattrib) */
/*----------------------------------------------------------------------------*/
#define	FFS_FILEATTRIBUTE_READONLY	0x01	/* Read only */
#define	FFS_FILEATTRIBUTE_HIDDEN	0x02	/* Hidden */
#define	FFS_FILEATTRIBUTE_SYSTEM	0x04	/* System */
#define FFS_FILEATTRIBUTE_DIRECTORY	0x10	/* Directory */
#define FFS_FILEATTRIBUTE_ARCHIVE	0x20	/* Archive */
/*----------------------------------------------------------------------------*/
#define FFS_SFNBUFSIZE		12
/*----------------------------------------------------------------------------*/
typedef struct
	{
	filesize_t		fsize;								/* File size */
	uint16_t		fdate;								/* Modified date */
	uint16_t		ftime;								/* Modified time */
	uint8_t			fattrib;							/* File attribute */
	ffs_char_t		altname[FFS_SFNBUFSIZE + 1];		/* Alternative file name */
	ffs_char_t		fname[FFS_CONFIG_MAXIMUMLFN + 1];	/* Primary file name */
	} ffs_fileinfo_t;
/*============================================================================*/
/* 'mkfs' parameter structure (mkfs_param_t) */
/*============================================================================*/
/* Values for field 'format' in structure 'mkfs_param_t'. */
/*----------------------------------------------------------------------------*/
#define FFS_FILESYSTEMFORMAT_FAT	0x01
#define FFS_FILESYSTEMFORMAT_FAT32	0x02
#define FFS_FILESYSTEMFORMAT_EXFAT	0x04
#define FFS_FILESYSTEMFORMAT_ANY	0x07
/*----------------------------------------------------------------------------*/
typedef struct
	{
	uint8_t			format;			/* Format option (FM_FAT, FM_FAT32, FM_EXFAT) */
	uint8_t			fatcopies;		/* Number of FAT copies */
	unsigned int	align;			/* Data area alignment (sectors) */
	unsigned int	rootentries;	/* Number of root directory entries */
	uint32_t		clustersize;	/* Cluster size (bytes) */
	} mkfs_param_t;
/*============================================================================*/
/*----------------------------------------------------------------------------*/
/* FatFs Module Application Interface                                         */
/*----------------------------------------------------------------------------*/
/*============================================================================*/
#if 0
int			FFS_AttachDrive				( const char *pdname, void *cookie, const ffs_diskio_t *drv );
int			FFS_DetachDrive				( const char *pdname );
int			FFS_GetAttachedDriversNbr	( void );
int			FFS_GetPhysicalDriveName	( int disknum, char *pdname );
/*============================================================================*/
#define	FFS_FINDPHYSICALDRIVE_INVALIDNAME	-2
#define	FFS_FINDPHYSICALDRIVE_NOTFOUND		-1
/*----------------------------------------------------------------------------*/
int			FFS_FindPhysicalDriveByName	( const char *Name );
#endif
/*============================================================================*/
/* Functions to manipulate the file system volumes. */
/*============================================================================*/
int			fdisk			( const ffs_char_t *pdname, const lba_t ptbl[], void *work );
int			mkfs			( const ffs_char_t *pdname, const mkfs_param_t *opt, void *work, size_t len );
int			mount			( const ffs_char_t *pdname, const ffs_char_t *ldname );
int			umount			( const ffs_char_t *ldname, int forced );

ssize_t		getfree			( const ffs_char_t *ldname );
int			getlabel		( const ffs_char_t *ldname, ffs_char_t *label, uint32_t *vsn );
int			setlabel		( const ffs_char_t *label );

int			ffs_getcwd		( const ffs_char_t *ldname, ffs_char_t *buff, size_t len );
int			chdir			( const ffs_char_t *path );
int			chdrive			( const ffs_char_t *ldname );
/*============================================================================*/
/* Functions to manipulate the data inside the files. They all take a file
   descriptor (fd), except for 'open', that takes the name of the file. */
/*============================================================================*/
/* Value for the third (optional) argument to 'open' to allow for concurrent
access to files. */
/*----------------------------------------------------------------------------*/
#define	ACCESS_EXCLUSIVE	0x15aacba0
#define	ACCESS_ALLOW_READ	0x15aacba1
#define	ACCESS_ALLOW_APPEND	0x15aacba2
#define	ACCESS_ALLOW_WRITE	0x15aacba3
/*----------------------------------------------------------------------------*/
#if         !defined _FREAD
#define	_FREAD		0x0001	/* read enabled */
#define	_FWRITE		0x0002	/* write enabled */
#define	_FAPPEND	O_APPEND	/* append (writes guaranteed at the end) */
#define	_FCREAT		O_CREAT	/* open with file create */
#define	_FTRUNC		O_TRUNC	/* open with truncation */
#define	_FEXCL		O_EXCL	/* error on open if file exists */

//#define	O_RDONLY	0		/* +1 == FREAD */
//#define	O_WRONLY	1		/* +1 == FWRITE */
//#define	O_RDWR		2		/* +1 == FREAD|FWRITE */
#endif  /*  !defined _FREAD */
/*----------------------------------------------------------------------------*/
int			open			( const ffs_char_t *path, long oflag, ... );
int			read			( int fd, void *ptr, size_t len );
int			write			( int fd, const void *ptr, size_t len );
int			fsync			( int fd );
int			close			( int fd );
#if         defined FFS_CONFIG_USELBA64 && FFS_CONFIG_USELBA64 != 0
_off64_t	lseek64			( int fd, _off64_t offset, int whence );
_off64_t	llseek			( int fd, _off64_t offset, int whence );
int			ftruncate64		( int fd, ffs_off_t len );
#endif  /*  defined FFS_CONFIG_USELBA64 && FFS_CONFIG_USELBA64 != 0 */
off_t		lseek			( int fd, off_t offset, int whence );
int			ftruncate		( int fd, off_t len );
int			expand			( int fd, ffs_off_t fsz, int opt );
//int			eof				( int fp );
/*============================================================================*/
/* Functions to access the meta-data of the files. */
/*============================================================================*/
int			ffs_fstat		( const ffs_char_t *path, ffs_fileinfo_t *fno );
int			utime			( const ffs_char_t *path, const ffs_fileinfo_t *fno );
/*============================================================================*/
/* Functions to manipulate the files or directories themselves. They all take
   the name of the file or directory */
/*============================================================================*/
int			mkdir			( const ffs_char_t *path, mode_t mode );
int			rename			( const ffs_char_t *path_old, const ffs_char_t *path_new );
int			unlink			( const ffs_char_t *path );
int			chmod			( const ffs_char_t *path, uint32_t attr );
/*============================================================================*/
/* Functions to access the information stored in the directories */
/*============================================================================*/
size_t		SizeOfDirObject	( void );
int			opendir			( void *dp, const ffs_char_t *path );
int			closedir		( void *dp );
int			readdir			( void *dp, ffs_fileinfo_t *fno );
int			findfirst		( void *dp, ffs_fileinfo_t *fno, const ffs_char_t *path, const ffs_char_t *pattern );
int			findnext		( void *dp, ffs_fileinfo_t *fno );
/*============================================================================*/
/*--------------------------------------------------------------*/
/* Additional Functions                                         */
/*--------------------------------------------------------------*/
/*============================================================================*/
/* LFN support functions (defined in ffsunicode.c) */
/*============================================================================*/
int			setcp			( uint16_t cp );
ffs_wchar_t	ffs_oem2uni		( ffs_wchar_t oem, uint16_t cp );	/* OEM code to Unicode conversion */
ffs_wchar_t	ffs_uni2oem		( uint32_t uni, uint16_t cp );		/* Unicode to OEM code conversion */
uint32_t	ffs_wtoupper	( uint32_t uni );					/* Unicode upper-case conversion */
/*============================================================================*/
#ifdef __cplusplus
}
#endif
/*============================================================================*/
#endif	/*	!defined _FFS_H__ */
/*============================================================================*/
