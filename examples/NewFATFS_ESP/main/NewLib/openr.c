/* Reentrant versions of open system call. */

#include <reent.h>
#include <unistd.h>
#include <fcntl.h>
#include <_syslist.h>
#include <stdarg.h>

/* Some targets provides their own versions of this functions.  Those
   targets should define REENTRANT_SYSCALLS_PROVIDED in TARGET_CFLAGS.  */

/* We use the errno variable used by the system dependent layer.  */
#undef errno
extern int errno;

/*
FUNCTION
	<<_open_r>>---Reentrant version of open
	
INDEX
	_open_r

SYNOPSIS
	#include <reent.h>
	int _open_r(struct _reent *<[ptr]>,
		    const char *<[file]>, int <[flags]>, int <[mode]>);

DESCRIPTION
	This is a reentrant version of <<open>>.  It
	takes a pointer to the global data block, which holds
	<<errno>>.
*/

int _open( const char *path, int oflag, int pmode );

int _open_r (struct _reent *ptr,
     const char *file,
     int flags,
     int mode)
{
  int ret;

  errno = 0;
  if ((ret = _open (file, flags, mode)) == -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}

int open( const char *path, int oflag, ... )
	{
	va_list argptr;
	int     pmode;

	va_start( argptr, oflag );	// __builtin_va_start(v,l)
	pmode	= va_arg( argptr, int );			// __builtin_va_arg(v,l)
	va_end( argptr );		// __builtin_va_end(v)

	return _open_r( _GLOBAL_REENT, path, oflag, pmode);
	}
