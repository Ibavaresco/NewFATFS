/*============================================================================*/
#include <stdio.h>
#include <ffs.h>
#include <ram_diskio.h>
#include <esp_log.h>
/*============================================================================*/
static int format( const char *pdname )
	{
	uint8_t	Buff[512];

	return mkfs( pdname, NULL, Buff, sizeof Buff );
	}
/*============================================================================*/
void app_main(void)
    {
	FFS_AttachDrive( "ramd", NULL, &RAMDisk_Driver );
	format( "ramd" );
	mount( "ramd", "A:" );

    static const char	FileName[]	= "Test.txt";
  	static const char	Msg[]		= "Task1 was here!\r\n";
	char				Buff[128];
	volatile int		len, total;
	int					fd;

	ESP_LOGI( "main", "Test" );

	while( 1 )
		{
		if(( fd = open( FileName, O_RDWR | O_CREAT | O_APPEND, ACCESS_ALLOW_APPEND )) != -1 )
			{
			total = 0;
			while(( len = write( fd, Msg, sizeof Msg - 1 )) > 0 )
			total += len;
			
			close( fd );
			}

		if(( fd = open( FileName, O_RDONLY, ACCESS_ALLOW_APPEND )) != -1 )
			{
			total	= 0;
			while(( len = read( fd, Buff, sizeof Buff )) > 0 )
			total += len;

			close( fd );
			}
		}
	}
/*============================================================================*/
