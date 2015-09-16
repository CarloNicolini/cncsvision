/*****************************************************************
Name:               Usblink.c

Description:
    This module defines the 6 low level driver routines for the
    PCI Interface Card.

        LinkOpen()
        LinkClose()
        LinkRead()
        LinkWrite()
        LinkStatus()
        LinkReset()

Library Name:

Include Files:


Modifications:


*****************************************************************/

/*****************************************************************
C Library Files Included
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

/*****************************************************************
ND Library Files Included
*****************************************************************/
#include <ndtypes.h>
#include <ndpack.h>
#include <ndopto.h>
#include <nderror.h>

/*****************************************************************
Project Files Included
*****************************************************************/
#ifdef HOST_WIN32
#include <windows.h>
#include "ftd2xx.h"
#else
#error "This file must be compiled for Windows 32"
#endif /* HOST_WIN32 */

/*****************************************************************
Application Files Included
*****************************************************************/

/*****************************************************************
Defines
*****************************************************************/
#define MAX_DELAY 50000

/*****************************************************************
External Variables and Routines
*****************************************************************/


/*****************************************************************
Internal Routines
*****************************************************************/
void    Pause( float fSec );

/*****************************************************************
Global Variables
*****************************************************************/

static char
	szSrvrName[256],
   *szNDErrorTitle = "Northern Digital OAPI Error";
static FT_HANDLE  s_hndl;

#define DEFAULT_USB_TYPE	5
#define DEFAULT_USB_ID		0x403DA77
#define DEFAULT_USB_LATENCY	2
#define DEFAULT_RESET_DELAY	1000

static int
	s_nOpenCount = 0,
	s_nUsbType      = DEFAULT_USB_TYPE,
	s_nUsbId        = DEFAULT_USB_ID,
	s_nUsbLatency   = DEFAULT_USB_LATENCY,
	s_nResetDelay   = DEFAULT_RESET_DELAY,
	s_nReadTimeout  = 10000,
	s_nWriteTimeout = 10000;

static AppParmInfo
    grUSBParms[] =
    {
        {
            "Type",
            &s_nUsbType,
            sizeof( s_nUsbType ),
            APP_PARM_INT | APP_PARM_OPTIONAL,
            0,
            0
        },
        {
            "Id",
            &s_nUsbId,
            sizeof( s_nUsbId ),
            APP_PARM_INT | APP_PARM_OPTIONAL,
            1,
            99999
        },
		{
            "Latency",
            &s_nUsbLatency,
            sizeof( s_nUsbLatency ),
            APP_PARM_INT | APP_PARM_OPTIONAL,
            2,
            255
        },
		{
            "ResetDelay",
            &s_nResetDelay,
            sizeof( s_nResetDelay ),
            APP_PARM_INT | APP_PARM_OPTIONAL,
            1,
            30000
        },
		{
            "ReadTimeOut",
            &s_nReadTimeout,
            sizeof( s_nReadTimeout ),
            APP_PARM_INT | APP_PARM_OPTIONAL,
            1,
            65535
        },
		{
            "WriteTimeOut",
            &s_nWriteTimeout,
            sizeof( s_nWriteTimeout ),
            APP_PARM_INT | APP_PARM_OPTIONAL,
            1,
            65535
        },
		{ NULL, 0 }
    };

/*****************************************************************
Static Routines
*****************************************************************/

/*****************************************************************
Name:               Pause

Input Values:
    float
        fSec        :Number of seconds to delay.

Output Values:
	None.

Return Value:
	None.

Description:
    Delay processing for the input number of seconds.

*****************************************************************/

void Pause( float fSec )
{
   clock_t end;

   end  = (clock_t)fSec * CLOCKS_PER_SEC + clock();
   while( clock() < end )
      ;
}


static FT_HANDLE OpenUSBDevice( int nType, int nId )
{
	FT_HANDLE	hndl = 0;	
	unsigned 	i;
	DWORD		dwDevs;
	FT_DEVICE_LIST_INFO_NODE *pNodes = 0;

	if( FT_CreateDeviceInfoList( &dwDevs) == FT_OK )
	{
		pNodes = (FT_DEVICE_LIST_INFO_NODE *)calloc( sizeof(FT_DEVICE_LIST_INFO_NODE) , dwDevs );
		if( FT_GetDeviceInfoList( pNodes, &dwDevs ) == FT_OK )
		{
			for( i = 0; i < dwDevs; i++ )
			{
				if( pNodes[i].Type == nType && pNodes[i].ID == nId )
				{
					if( FT_OpenEx( pNodes[i].Description, FT_OPEN_BY_DESCRIPTION, &hndl ) != FT_OK )
					{
						hndl = 0;
						goto cleanup;
					} /* if */
					if( FT_SetLatencyTimer( hndl, s_nUsbLatency ) != FT_OK ||
						FT_SetTimeouts( hndl, s_nReadTimeout, s_nWriteTimeout ) != FT_OK )
					{
						FT_Close( s_hndl );
						hndl = 0;
						goto cleanup;
					}
					break;
				} /* if */
			} /* for */
		} /* if */
	} /* if */

cleanup:
	if( pNodes )
		free( pNodes );

	return hndl;
}

/***************************************************************************

Routine:           LinkOpen

Inputs:
    None

Returns:
    boolean               : TRUE if link correctly initialized.

Description:
    Initializes the OPTOTRAK communications link:
        Determines the address of the PC interface card
        Initializes the C012 chip.

***************************************************************************/

NDI_DECL1 boolean NDI_DECL2 LinkOpen( void )
{
	boolean
		bRet = FALSE;

	/*
	 * check to see if the link is open
	 */
	if( s_nOpenCount == 0 )
	{
	    if( !ReadAppParms( "optotrak", "USB Options", grUSBParms ) )
	    {
			return FALSE;
		} /* if */

		s_hndl = OpenUSBDevice( s_nUsbType, s_nUsbId );
		if( !s_hndl )
		{
			return FALSE;
		} /* if */
    } /* if */
    
	s_nOpenCount++;
#ifdef MESSAGE_BOX_OUTPUT
	{
		char szBuff[ 100 ];
		sprintf( szBuff, "LinkOpen: Count = %d", s_nOpenCount );
		MessageBox( 0, szBuff, "LinkOpen", MB_OK );
	}
#endif

	return TRUE;
}

/***************************************************************************

Routine:           LinkWrite

Inputs:
    void *pBuff           : Pointer to data buffer.
    unsigned uBytes       : Number of bytes to send to the link.

Returns:
    unsigned              : Number of bytes actually written to the link.

Description:
    Attempts to send "uBytes" bytes of data starting at address "pBuff"
    to the OPTOTRAK.

***************************************************************************/

NDI_DECL1 unsigned int NDI_DECL2 LinkWrite( void *pBuff, unsigned int uBytes )
{
	DWORD	dwWritten;

	if( s_hndl )
	{
		if( FT_Write( s_hndl, pBuff, uBytes, &dwWritten ) == FT_OK )
			return dwWritten;
	} /* if */
	return 0;
}

/***************************************************************************

Routine:           LinkRead

Inputs:
    void *pData           : Pointer to a buffer of at least "uSize" bytes
    unsigned uSize        : Number of bytes to read into the buffer.

Returns:
    unsigned              : Number of bytes actually read into the buffer.

Description:
    Attempts to read a "uSize" byte packet of data from the OPTOTRAK.  The
    data is stored in the buffer specified by "pData".

***************************************************************************/

NDI_DECL1 unsigned int NDI_DECL2 LinkRead( void *pData, unsigned int uSize )
{
	DWORD	dwRead;

	if( s_hndl )
	{
		if( FT_Read( s_hndl, pData, uSize, &dwRead ) == FT_OK )
			return dwRead;
	} /* if */
	return 0;
}

/***************************************************************************

Routine:           LinkClose

Inputs:
    None

Returns:
    void

Description:
    Closes the communications channel to the OPTOTRAK.

***************************************************************************/
NDI_DECL1 boolean NDI_DECL2 LinkClose( void )
{
#ifdef MESSAGE_BOX_OUTPUT
	{
		char szBuff[ 100 ];
		sprintf( szBuff, "LinkClose: Count = %d", s_nOpenCount );
		MessageBox( 0, szBuff, "LinkClose", MB_OK );
	}
#endif

	/* 
	 * Close regardless of the Open Count since there's no need to keep the connection open.
	 * Note that we were originally keeping the connection open until Open Count was 1 but
	 * for some unknown reason, s_hndl and s_nOpenCount were being reset to 0 between the 
	 * Close and Open count (resulting in Open errors).
	 */
	if( s_hndl )
	{
		FT_Close( s_hndl );
		s_hndl = 0;
	}

	if( s_nOpenCount > 0 )
		s_nOpenCount = 0;

    return TRUE;
}

/***************************************************************************

Routine:           LinkStatus

Inputs:
    boolean *pbReadable   : pointer to storage for Readable status
    boolean *pbWriteable  : pointer to storage for Writeable status

Returns:
    void

Description:
    Determines whether or not the OPTOTRAK system is ready to accept data
    and whether there is data available from the OPTOTRAK.

***************************************************************************/
NDI_DECL1 boolean NDI_DECL2 LinkStatus( boolean *pbReadable, boolean *pbWriteable )
{
	DWORD	dwRxStat;
	DWORD	dwTxStat;
	DWORD	dwEvStat;

	*pbReadable = *pbWriteable = FALSE;
	if( s_hndl )
	{
		if( FT_GetStatus( s_hndl, &dwRxStat, &dwTxStat, &dwEvStat ) == FT_OK )
		{
			*pbReadable = (dwRxStat > 0) ? TRUE : FALSE;
			*pbWriteable = (dwTxStat > 0) ? FALSE : TRUE;
			return TRUE;
		} /* if */
	} /* if */
    return FALSE;
}

/***************************************************************************

Routine:           LinkReset

Inputs:
    None

Returns:
    void

Description:
    Resets the OPTOTRAK system.

***************************************************************************/
NDI_DECL1 boolean NDI_DECL2 LinkReset( void )
{
	if( s_hndl )
	{
		FT_ResetPort( s_hndl );
		FT_Close( s_hndl );
		Sleep( s_nResetDelay );

		s_hndl = OpenUSBDevice( s_nUsbType, s_nUsbId );
		if( s_hndl )
		{
			return TRUE;
		}
	}
	return FALSE;
}

