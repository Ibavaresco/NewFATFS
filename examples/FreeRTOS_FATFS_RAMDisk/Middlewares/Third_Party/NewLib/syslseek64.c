/* connector for lseek */

#include <reent.h>
#include <unistd.h>

_off64_t _lseek64_r (struct _reent *ptr, int fd, _off64_t pos, int whence);


_off64_t lseek64( int fd, _off64_t pos, int whence )
	{
	return _lseek64_r( _REENT, fd, pos, whence );
	}

_off64_t llseek( int fd, _off64_t pos, int whence )
	{
	return _lseek64_r( _REENT, fd, pos, whence );
	}
