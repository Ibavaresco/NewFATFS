/* connector for chdir */

#include <reent.h>
#include <unistd.h>

int _mkdir_r( struct _reent *ptr, const char *path, int mode );

int mkdir( const char *path, int mode )
	{
	return _mkdir_r( _REENT, path, mode );
	}
