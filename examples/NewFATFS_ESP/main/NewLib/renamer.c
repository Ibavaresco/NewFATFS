/* Reentrant version of rename system call.  */

#include <reent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <_syslist.h>

/* Some targets provides their own versions of these functions.  Those
   targets should define REENTRANT_SYSCALLS_PROVIDED in TARGET_CFLAGS.  */

#ifdef _REENT_ONLY
#ifndef REENTRANT_SYSCALLS_PROVIDED
#define REENTRANT_SYSCALLS_PROVIDED
#endif
#endif

#ifndef REENTRANT_SYSCALLS_PROVIDED

/* We use the errno variable used by the system dependent layer.  */
#undef errno
extern int errno;

/*
FUNCTION
	<<_rename_r>>---Reentrant version of rename
	
INDEX
	_rename_r

SYNOPSIS
	#include <reent.h>
	int _rename_r(struct _reent *<[ptr]>,
		const char *<[old]>, const char *<[new]>);

DESCRIPTION
	This is a reentrant version of <<rename>>.  It
	takes a pointer to the global data block, which holds
	<<errno>>.
*/

int _rename( const char *old, const char *new );

int _rename_r( struct _reent *ptr, const char *old, const char *new )
    {
    int ret = 0;

    errno = 0;
    if(( ret = _rename ( old, new )) == -1 && errno != 0 )
        ptr->_errno = errno;

    return ret;
    }

int rename( const char *old, const char *new )
    {
    return _rename_r( _GLOBAL_REENT, old, new );
    }

#endif /* ! defined (REENTRANT_SYSCALLS_PROVIDED) */
