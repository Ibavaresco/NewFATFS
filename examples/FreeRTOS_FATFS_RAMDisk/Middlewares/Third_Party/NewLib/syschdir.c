/* connector for chdir */

#include <reent.h>
#include <unistd.h>

int _chdir_r( struct _reent *ptr, const char *path );

int chdir( const char *path )
	{
	return _chdir_r( _REENT, path );
	}
