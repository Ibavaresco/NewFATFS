/*============================================================================*/
/* <https://github.com/Ibavaresco/NewFATFS> */
/*============================================================================*/
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <SimpleRTOS.h>
#include <ffs.h>
#include <ram_diskio.h>
/*============================================================================*/
#if 1
/*============================================================================*/
// PIC32MK1024MCF100 Configuration Bit Settings

// 'C' source line config statements

// DEVCFG3
#pragma config USERID		= 0xFFFF			// Enter Hexadecimal value (Enter Hexadecimal value)
#pragma config PWMLOCK		= OFF				// PWM IOxCON lock (PWM IOxCON register writes accesses are not locked or protected)
#pragma config FUSBIDIO2	= OFF				// USB2 USBID Selection (USBID pin is controlled by the port function)
#pragma config FVBUSIO2		= OFF				// USB2 VBUSON Selection bit (VBUSON pin is controlled by the port function)
#pragma config PGL1WAY		= OFF				// Permission Group Lock One Way Configuration bit (Allow multiple reconfigurations)
#pragma config PMDL1WAY		= OFF				// Peripheral Module Disable Configuration (Allow multiple reconfigurations)
#pragma config IOL1WAY		= OFF				// Peripheral Pin Select Configuration (Allow multiple reconfigurations)
#pragma config FUSBIDIO1	= OFF				// USB1 USBID Selection (USBID pin is controlled by the port function)
#pragma config FVBUSIO1		= OFF				// USB2 VBUSON Selection bit (VBUSON pin is controlled by the port function)

// DEVCFG2
#pragma config FPLLIDIV		= DIV_1				// System PLL Input Divider (1x Divider)
#pragma config FPLLRNG		= RANGE_5_10_MHZ	// System PLL Input Range (5-10 MHz Input)
#pragma config FPLLICLK		= PLL_POSC			// System PLL Input Clock Selection (POSC is input to the System PLL)
#pragma config FPLLMULT		= MUL_60			// System PLL Multiplier (PLL Multiply by 60)
#pragma config FPLLODIV		= DIV_4				// System PLL Output Clock Divider (4x Divider)

#if			defined __32MK1024MCF100__

#pragma config VBATBOREN	= OFF				// VBAT BOR Enable (Disable ZPBOR during VBAT Mode)
#pragma config DSBOREN		= OFF				// Deep Sleep BOR Enable (Disable ZPBOR during Deep Sleep Mode)
#pragma config DSWDTPS		= DSPS32			// Deep Sleep Watchdog Timer Postscaler (1:2^36)
#pragma config DSWDTOSC		= LPRC				// Deep Sleep WDT Reference Clock Selection (Select LPRC as DSWDT Reference clock)
#pragma config DSWDTEN		= OFF				// Deep Sleep Watchdog Timer Enable (Enable DSWDT during Deep Sleep Mode)
#pragma config FDSEN		= ON				// Deep Sleep Enable (Enable DSEN bit in DSCON)

#endif	/*	defined __32MK1024MCF100__ */


#pragma config BORSEL		= HIGH				// Brown-out trip voltage (BOR trip voltage 2.1v (Non-OPAMP deviced operation))
#pragma config UPLLEN		= OFF				// USB PLL Enable (USB PLL Disabled)

// DEVCFG1
#pragma config FNOSC		= SPLL				// Oscillator Selection Bits (Low Power RC Osc (LPRC))
#pragma config DMTINTV		= WIN_127_128		// DMT Count Window Interval (Window/Interval value is 127/128 counter value)
#pragma config FSOSCEN		= OFF				// Secondary Oscillator Enable (Disable Secondary Oscillator)
#pragma config IESO			= ON				// Internal/External Switch Over (Enabled)
#pragma config POSCMOD		= HS				// Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC		= OFF				// CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FCKSM		= CSECME			// Clock Switching and Monitor Selection (Clock Switch Enabled, FSCM Enabled)
#pragma config WDTPS		= PS1048576			// Watchdog Timer Postscaler (1:1048576)
#pragma config WDTSPGM		= STOP				// Watchdog Timer Stop During Flash Programming (WDT stops during Flash programming)
#pragma config WINDIS		= NORMAL			// Watchdog Timer Window Mode (Watchdog Timer is in non-Window mode)
#pragma config FWDTEN		= OFF				// Watchdog Timer Enable (WDT Disabled)
#pragma config FWDTWINSZ	= WINSZ_25			// Watchdog Timer Window Size (Window size is 25%)
#pragma config DMTCNT		= DMT31				// Deadman Timer Count Selection (2^31 (2147483648))
#pragma config FDMTEN		= OFF				// Deadman Timer Enable (Deadman Timer is disabled)

// DEVCFG0
#pragma config DEBUG		= OFF				// Background Debugger Enable (Debugger is disabled)
#pragma config JTAGEN		= OFF				// JTAG Enable (JTAG Disabled)
#pragma config ICESEL		= ICS_PGx1			// ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config TRCEN		= OFF				// Trace Enable (Trace features in the CPU are disabled)
#pragma config BOOTISA		= MIPS32			// Boot ISA Selection (Boot code and Exception code is MIPS32)
#pragma config FSLEEP		= OFF				// Flash Sleep Mode (Flash is powered down when the device is in Sleep mode)
#pragma config DBGPER		= PG_ALL			// Debug Mode CPU Access Permission (Allow CPU access to all permission regions)
#pragma config SMCLR		= MCLR_NORM			// Soft Master Clear Enable (MCLR pin generates a normal system Reset)

#if			defined __32MK1024MCM100__

#pragma config SOSCGAIN		= G2				// Secondary Oscillator Gain Control bits (2x gain setting)
#pragma config POSCGAIN		= G3				// Primary Oscillator Gain Control bits (Gain Level 3 (highest))

#endif	/*	defined __32MK1024MCM100__ */

#pragma config SOSCBOOST	= ON				// Secondary Oscillator Boost Kick Start Enable bit (Boost the kick start of the oscillator)
#pragma config POSCBOOST	= ON				// Primary Oscillator Boost Kick Start Enable bit (Boost the kick start of the oscillator)
#pragma config EJTAGBEN		= NORMAL			// EJTAG Boot Enable (Normal EJTAG functionality)

// DEVCP
#if			defined __DEBUG
#pragma config CP			= OFF
#warning ATENCAO: !!! Modo debug, protecao de codigo desligada !!!
#else	/*	defined __DEBUG */
#pragma config CP			= ON
#endif	/*	defined __DEBUG */

// SEQ
#pragma config TSEQ			= 0xFFFF			// Boot Flash True Sequence Number (Enter Hexadecimal value)
#pragma config CSEQ			= 0xFFFF			// Boot Flash Complement Sequence Number (Enter Hexadecimal value)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
/*============================================================================*/
#else
/*============================================================================*/
// PIC32MZ2048EFG100 Configuration Bit Settings

// 'C' source line config statements

// DEVCFG3
#pragma config USERID		= 0xFFFF          // Enter Hexadecimal value (Enter Hexadecimal value)
#pragma config FMIIEN		= OFF             // Ethernet RMII/MII Enable (RMII Enabled)
#pragma config FETHIO		= ON              // Ethernet I/O Pin Select (Default Ethernet I/O)
#pragma config PGL1WAY		= OFF            // Permission Group Lock One Way Configuration (Allow multiple reconfigurations)
#pragma config PMDL1WAY		= OFF           // Peripheral Module Disable Configuration (Allow multiple reconfigurations)
#pragma config IOL1WAY		= OFF            // Peripheral Pin Select Configuration (Allow multiple reconfigurations)
#pragma config FUSBIDIO		= OFF           // USB USBID Selection (Controlled by Port Function)

// DEVCFG2
#pragma config FPLLIDIV		= DIV_3         // System PLL Input Divider (3x Divider)
#pragma config FPLLRNG		= RANGE_5_10_MHZ // System PLL Input Range (5-10 MHz Input)
#pragma config FPLLICLK		= PLL_POSC      // System PLL Input Clock Selection (POSC is input to the System PLL)
#pragma config FPLLMULT		= MUL_50        // System PLL Multiplier (PLL Multiply by 50)
#pragma config FPLLODIV		= DIV_2         // System PLL Output Clock Divider (2x Divider)
#pragma config UPLLFSEL		= FREQ_24MHZ    // USB PLL Input Frequency Selection (USB PLL input is 24 MHz)

// DEVCFG1
#pragma config FNOSC		= FRCDIV             // Oscillator Selection Bits (System PLL)
#pragma config DMTINTV		= WIN_127_128    // DMT Count Window Interval (Window/Interval value is 127/128 counter value)
#pragma config FSOSCEN		= OFF            // Secondary Oscillator Enable (Disable SOSC)
#pragma config IESO			= ON                // Internal/External Switch Over (Enabled)
#pragma config POSCMOD		= HS             // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC		= OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FCKSM		= CSECMD           // Clock Switching and Monitor Selection (Clock Switch Enabled, FSCM Disabled)
#pragma config WDTPS		= PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config WDTSPGM		= STOP           // Watchdog Timer Stop During Flash Programming (WDT stops during Flash programming)
#pragma config WINDIS		= NORMAL          // Watchdog Timer Window Mode (Watchdog Timer is in non-Window mode)
#pragma config FWDTEN		= OFF             // Watchdog Timer Enable (WDT Disabled)
#pragma config FWDTWINSZ	= WINSZ_25     // Watchdog Timer Window Size (Window size is 25%)
#pragma config DMTCNT		= DMT31           // Deadman Timer Count Selection (2^31 (2147483648))
#pragma config FDMTEN		= OFF             // Deadman Timer Enable (Deadman Timer is disabled)

// DEVCFG0
#pragma config DEBUG		= ON               // Background Debugger Enable (Debugger is enabled)
#pragma config JTAGEN		= OFF             // JTAG Enable (JTAG Disabled)
#pragma config ICESEL		= ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config TRCEN		= OFF              // Trace Enable (Trace features in the CPU are disabled)
#pragma config BOOTISA		= MIPS32         // Boot ISA Selection (Boot code and Exception code is MIPS32)
#pragma config FECCCON		= OFF_UNLOCKED   // Dynamic Flash ECC Configuration (ECC and Dynamic ECC are disabled (ECCCON bits are writable))
#pragma config FSLEEP		= OFF             // Flash Sleep Mode (Flash is powered down when the device is in Sleep mode)
#pragma config DBGPER		= PG_ALL          // Debug Mode CPU Access Permission (Allow CPU access to all permission regions)
#pragma config SMCLR		= MCLR_NORM        // Soft Master Clear Enable bit (MCLR pin generates a normal system Reset)
#pragma config SOSCGAIN		= GAIN_2X       // Secondary Oscillator Gain Control bits (2x gain setting)
#pragma config SOSCBOOST	= ON           // Secondary Oscillator Boost Kick Start Enable bit (Boost the kick start of the oscillator)
#pragma config POSCGAIN		= GAIN_2X       // Primary Oscillator Gain Control bits (2x gain setting)
#pragma config POSCBOOST	= ON           // Primary Oscillator Boost Kick Start Enable bit (Boost the kick start of the oscillator)
#pragma config EJTAGBEN		= REDUCED       // EJTAG Boot (Reduced EJTAG functionality)

// DEVCP0
#pragma config CP			= OFF                 // Code Protect (Protection Disabled)

// SEQ3
#pragma config TSEQ			= 0xFFFF            // Boot Flash True Sequence Number (Enter Hexadecimal value)
#pragma config CSEQ			= 0xFFFF            // Boot Flash Complement Sequence Number (Enter Hexadecimal value)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
/*============================================================================*/
#endif
/*============================================================================*/
#include <xc.h>

#include "PortPIC32Internals.h"
/*============================================================================*/
int open( const char *buf, long flags, ... )
	{
	int		_open( const char *buf, long flags, int pmode );
	va_list	argptr;
	int		pmode;

	va_start( argptr, flags );	// __builtin_va_start(v,l)
	pmode	= va_arg( argptr, int );			// __builtin_va_arg(v,l)
	va_end( argptr );		// __builtin_va_end(v)

	return _open( buf, flags, pmode );
	}
/*============================================================================*/
int write( int fd, const void *buf, size_t len )
	{
	int _write( int fd, const void *buf, size_t len );

	return _write( fd, buf, len );
	}
/*============================================================================*/
int read( int fd, void *buf, size_t len )
	{
	int _read( int fd, void *buf, size_t len );

	return _read( fd, buf, len );
	}
/*============================================================================*/
int close( int fd )
	{
	int _close( int fd );

	return _close( fd );
	}
/*============================================================================*/
off_t lseek( int fd, off_t offset, int whence )
	{
	off_t _lseek( int fd, off_t offset, int whence );

	return _lseek( fd, offset, whence );
	}
/*============================================================================*/
#define MAX_TASKS   4
#define STACK_SIZE  4096
/*============================================================================*/
/* Each task needs a context... */
context_t   Contexts[MAX_TASKS];
/* ...and a stack. */
#if			defined __DEBUG
/* Aligning to a 16-byte boundary just for convenience during debugging. */
__attribute__(( aligned( 16 )))
#endif	/*	defined __DEBUG */
uint8_t     Stacks[MAX_TASKS][STACK_SIZE];
/*============================================================================*/
static const char	FileName[]	= "Test.txt";
/*============================================================================*/
#if 0
static int ProcessC( int fd, char *Buff, int BuffLen )
	{
	static const char	Msg[]	= "NewFATFS test on PIC32MZ2048 with SimpleRTOS";
	int					n;

	if( write( fd, Msg, sizeof Msg - 1 ) != sizeof Msg - 1 )
		return -5;

	if( lseek( fd, SEEK_SET, 0 ) != 0 )
		return -6;

	n	= read( fd, Buff, BuffLen );
	if( n <= 0 )
		return -7;

	if( n != sizeof Msg - 1 )
		return -8;
	
	if( strncmp( Buff, Msg, sizeof Msg - 1 ) != 0 )
		return -9;
	
	return n;
	}
/*============================================================================*/
static int ProcessB( void )
	{
	char	Buff[128];
	int		fd, result;

	if( setlabel( "A:RAMDISK" ) != 0 )
		return -2;

	if( getlabel( "A:", Buff, NULL ) != 0 )
		return -3;

	if(( fd = open( "TEST.TXT", O_RDWR | O_CREAT, ACCESS_EXCLUSIVE )) == -1 )
		return -4;

	result	= ProcessC( fd, Buff, sizeof Buff );

	close( fd );

	return result;
	}
/*============================================================================*/
static int Process( void )
	{
	int	result;

	if( mount( "ramd", "A:" ) != 0 )
		return -1;

	result	= ProcessB();

	umount( "A:", 0 );

	return result;
	}
/*============================================================================*/
int Process2( void )
	{
	char	Buff[128];
	FILE	*f;
	int		n;

	if( mount( "ramd", "A:" ) != 0 )
		return -1;
	
	f	= fopen( "Test.txt", "w+" );
	if( f == NULL )
		return -2;
	
	n	= fwrite( "Test", 1, 4, f );
	
	fclose( f );

	f	= fopen( "Test.txt", "r+" );
	if( f == NULL )
		return -3;
	
	n = fread( Buff, 1, 4, f );
	
	fclose( f );
	}
#endif
/*============================================================================*/
static int format( const char *pdname )
	{
	uint8_t	Buff[512];

	return mkfs( pdname, NULL, Buff, sizeof Buff );
	}
/*============================================================================*/
static void Task1( void *p )
    {
	static const char	Msg[]	= "Task1 was here!\r\n";

	FFS_AttachDrive( "ramd", NULL, &RAMDisk_Driver );
	format( "ramd" );
	mount( "ramd", "A:" );

    while( 1 )
        {
		volatile int	len, total;
		tickcount_t		t1;
		int				fd;

		t1	= GetTickCount();
		
		while(( fd = open( FileName, O_RDWR | O_APPEND, ACCESS_ALLOW_APPEND )) == -1 )
			vSleep( 1 );
		
		total = 0;
		while(( len = write( fd, Msg, sizeof Msg - 1 )) > 0 )
			{
			total += len;
			Yield();
			}
		
		close( fd );

		t1	= GetTickCount() - t1;

		while( 1 )
			vSleep( 1 );
        }
    }
/*============================================================================*/
static void Task2( void *p )
    {
	static const char	Msg[]	= "Task2 was here!\r\n";

    while( 1 )
        {
		volatile int	len, total;
		tickcount_t		t1;
		int				fd;

		while(( fd = open( FileName, O_RDWR | O_CREAT | O_APPEND, ACCESS_ALLOW_APPEND )) == -1 )
			vSleep( 1 );

		t1	= GetTickCount();

		total	= 0;
		while(( len = write( fd, Msg, sizeof Msg - 1 )) > 0 )
			{
			total += len;
			Yield();
			}

		t1	= GetTickCount() - t1;

		close( fd );

		while( 1 )
			vSleep( 1 );
        }
    }
/*============================================================================*/
static void Task3( void *p )
    {
    while( 1 )
        {
		volatile tickcount_t t1;
		volatile int	len, total;
		char			Buff[128];
		int				fd;

		while(( fd = open( FileName, O_RDONLY, ACCESS_ALLOW_APPEND )) == -1 )
			vSleep( 1 );

		t1	= GetTickCount();

		total	= 0;
		while(( len = read( fd, Buff, sizeof Buff )) > 0 )
			total += len;

		t1	= GetTickCount() - t1;

		close( fd );

		vSleep( 1 );
        }
    }
/*============================================================================*/
static void Task4( void *p )
    {
    while( 1 )
		/* At least one task must never sleep or block, and ideally it must have
		   the lesser priority. It is the 'idle' task. So we will just 'Yield'. */
        Yield();
    }
/*============================================================================*/
int main( void )
    {
	int	i;

	CHECONbits.PFMWS	= 3;
	CHECONbits.PREFEN	= 1;
	CHECONbits.PFMAWSEN	= 1;

    INTCONbits.MVEC		= 1;

	/* Initialize the guts of the RTOS. */
    InitRTOS( Contexts, MAX_TASKS );

	/* Let's fill the stacks with letters to help with debugging. */
	for( i = 0; i < MAX_TASKS; i++ )
		memset( Stacks[i], 'A' + i, sizeof Stacks[i] );

	/* Create the tasks that will run concurrently. We don't need to create them
	   all here, tasks can create and destroy tasks too. */
    CreateTask( Stacks[0], sizeof Stacks[0], NULL, Task1, 1, NULL );
    CreateTask( Stacks[1], sizeof Stacks[1], NULL, Task2, 1, NULL );
    CreateTask( Stacks[2], sizeof Stacks[2], NULL, Task3, 1, NULL );
    CreateTask( Stacks[3], sizeof Stacks[3], NULL, Task4, 0, NULL );
    
	/* Start the RTOS. From Now on the main() will not run anymore. */
    StartRTOS( 200000 );
    }
/*============================================================================*/
