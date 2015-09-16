/*****************************************************************
Name:               PCIlink.c

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

    SCO-00-0001: File created (modified from pclink.c).

*****************************************************************/

/*****************************************************************
C Library Files Included
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

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

/*****************************************************************
Application Files Included
*****************************************************************/

/*****************************************************************
Defines
*****************************************************************/
#define DEFAULT_CTRL_PORT	9999
#define DEFAULT_DATA_PORT	9998

/*
 * Timeouts are in milliseconds
 */
#define DEFAULT_RESET_DELAY	2000
#define DEFAULT_TIMEOUT		5000

#define INVALID_SOCKET		-1

/*****************************************************************
External Variables and Routines
*****************************************************************/
static void
	(*pfnLogErr)( char *psz, ... ) = 0;

/*****************************************************************
Internal Routines
*****************************************************************/
void	Pause( float fSec );
void	ErrorMsg( char *pFormatString, ... );
static void CloseSocket(void);

/*****************************************************************
Global Variables
*****************************************************************/

static int
	nSocket = INVALID_SOCKET;

static char
	szSrvrName[256],
	*szNDErrorTitle = "Northern Digital OAPI Error";

static int
	nCtrlPort = DEFAULT_CTRL_PORT,
	nDataPort = DEFAULT_DATA_PORT,
	nTimeOut =  DEFAULT_TIMEOUT,
	nResetDelay = DEFAULT_RESET_DELAY;

static AppParmInfo grEtherParms[] =
{
	// AppParmInfo tables should be limited to line per entry, to permit searching and parsing.
	{ "Server Address",	szSrvrName,		sizeof( szSrvrName ),	APP_PARM_STRING | APP_PARM_OPTIONAL,	0,	0	},
	{ "Control Port",	&nCtrlPort,		sizeof( nCtrlPort ),	APP_PARM_INT | APP_PARM_OPTIONAL,		1,	65535	},
	{ "Data Port",		&nDataPort,		sizeof( nDataPort ),	APP_PARM_INT | APP_PARM_OPTIONAL,		1,	65535	},
	{ "Data Timeout",	&nTimeOut,		sizeof(nTimeOut),		APP_PARM_INT | APP_PARM_OPTIONAL,		1,	30000	},	
	{ "Reset Delay",	&nResetDelay,	sizeof( nResetDelay ),	APP_PARM_INT | APP_PARM_OPTIONAL,		0,	30000	},
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
	long
		lSec = (long)fSec,
		lUSec = (long)((fSec - lSec) * 1000000);

	sleep( lSec );
	usleep( lUSec );
}

/*****************************************************************
Name:                   ErrorMsg

Input Values:
	char
		*pFormatString :Format string to use with the error message.

Output Values:
	None.

Returned Value:
	None.

Description:

This routine is used to handle error messages from ND libraries.

*****************************************************************/

void ErrorMsg( char *pFormatString, ... )
{
	char szBuffer[MAX_ERROR_STRING_LENGTH + 1];
	va_list va;

	if( pfnLogErr )
	{
		va_start( va, pFormatString );
		vsprintf( szBuffer, pFormatString, va );
		va_end( va );

		(*pfnLogErr)( szBuffer );
	}
}

/*****************************************************************
Name:                   LinkSetErrorFunction

Input Values:
	void (*pfn)( char *psz, ... )
				Pointer to ErrorLogging function.

Output Values:
	None.

Returned Value:
	None.

Description:
	This routine opens the data socket to the OPTOTRAK server.

*****************************************************************/
void LinkSetErrorFunction( void (*pfn)( char *psz, ... ) )
{
	pfnLogErr = pfn;
}


/*****************************************************************
Name:                   bOpenSocket

Input Values:
	none

Output Values:
	None.

Returned Value:
    booelan:		TRUE if data socket successfully opened.

Description:
	This routine opens the data socket to the OPTOTRAK server.

*****************************************************************/

static boolean bOpenSocket( void )
{
	boolean
		bRet = FALSE;
	struct sockaddr_in
		sockAddr;
	int
		nSockOpt = 1;

	/*
	 * if there is an open socket, close it first!
	 */
	if( nSocket != INVALID_SOCKET )
	{
		close( nSocket );
		nSocket = INVALID_SOCKET;
	} /* if */


	nSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if( nSocket == INVALID_SOCKET )
	{
		ErrorMsg( "Unable to create socket" );
		goto LINK_OPEN_EXIT;
	} /* if */

	if( setsockopt( nSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&nSockOpt, sizeof(nSockOpt) ) != 0 )
	{
		ErrorMsg( "Unable to set NODELAY.  Performance may be impacted" );
	} /* if */

	memset( &sockAddr, 0, sizeof(sockAddr) );

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr( szSrvrName );

	if( sockAddr.sin_addr.s_addr == -1 )
	{
		struct hostent *phost;
		phost = gethostbyname( szSrvrName );
		if( phost != NULL )
		{
			sockAddr.sin_addr.s_addr = ((struct in_addr *)phost->h_addr)->s_addr;
		}
		else
		{
			ErrorMsg( "Optotrak '%s' could not be found", szSrvrName );
			goto LINK_OPEN_EXIT; 
		} /* else */
	}

	sockAddr.sin_port = htons( (unsigned short)nDataPort );

	if( connect( nSocket, (struct sockaddr *)&sockAddr, sizeof(sockAddr) ) != 0 )
	{
		ErrorMsg( "Unable to connect to OPTOTRAK" );
		goto LINK_OPEN_EXIT;
	} /* if */

	bRet = TRUE;

LINK_OPEN_EXIT:
	if( !bRet )
	{
		if( nSocket != INVALID_SOCKET )
		{
			close( nSocket );
			nSocket = INVALID_SOCKET;
		} /* if */
	} /* if */

	return bRet;
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

		Note: Reference counting was removed to fix 20362.

***************************************************************************/

NDI_DECL1 boolean NDI_DECL2 LinkOpen( void )
{
	boolean
		bRet = FALSE;

	/*
	 * check to see if the link is open
	 */
	if( nSocket == INVALID_SOCKET )
	{
		if( !ReadAppParms( "OPTOTRAK", "Ethernet Options", grEtherParms ) )
		{
			ErrorMsg( "Etherlink::LinkOpen() : Unable to read the Ethernet parameters." );
			return FALSE;
		} /* if */

		if( !bOpenSocket() )
		{
			ErrorMsg( "Etherlink::LinkOpen() : Failed opening socket" );
			return FALSE;
		} /* if */
    } /* if */

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
	unsigned
		uReq = uBytes,
		uCnt = 0;
	unsigned char
		*pch;
	struct timeval
		tv;
	fd_set
		dtWrFds;
	int
		nWritten;

	/*
	 * Make sure that the socket is open
	 */
	if( nSocket == INVALID_SOCKET )
	{
		ErrorMsg( "Etherlink::LinkWrite() : Socket is not open" );
		return uCnt;
	} /* if */

	pch = (unsigned char *)pBuff;

	while( uBytes )
	{
		nWritten = send( nSocket, pch, uBytes, 0 );
		if( nWritten <= 0 )
		{
			ErrorMsg( "Etherlink::LinkWrite() : Unable to write to socket" );
			CloseSocket();
			break;
		}

		pch += nWritten;
		uCnt += nWritten;
		uBytes -= nWritten;
	} /* while */

#if 0
	if( uReq != uCnt )
	{
		char szTmp[100];
		sprintf( szTmp, "LinkWrite Returned %d bytes", uCnt );
	}
#endif
    return uCnt;
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
	unsigned
		uReq = uSize,
		uCnt = 0;
	unsigned char
		*pch;
	struct timeval
		tv;
	fd_set
		dtRdFds;
	int
		nAvail,
		nRead,
		nErr;

	/*
	 * Make sure that the socket is open
	 */
	if( nSocket == INVALID_SOCKET )
	{
		return uCnt;
	}

	pch = (unsigned char *)pData;

	while( uSize )
	{
		FD_ZERO( &dtRdFds );
		FD_SET( nSocket, &dtRdFds );

		/*
		 * Timeout after nTimeOut milliseconds
		 */
		tv.tv_sec = nTimeOut / 1000;
		tv.tv_usec = (nTimeOut - 1000 * tv.tv_sec) * 1000;

		/*
		 * If select returns anything but '1', it means there there was a timeout or an error.
		 */
		nErr = select( nSocket + 1, &dtRdFds, 0, 0, &tv );
		if( nErr != 1 )
		{
			ErrorMsg( "Etherlink::LinkRead() : Unable to determine socket's status" );
			CloseSocket();
			break;
		} /* if */

		/*
		 * Peek into the receive buffer to determine how many bytes are available.
		 * If <= 0, then the socket has been closed or there is an error
		 */
		nAvail = recv( nSocket, pch, uSize, MSG_PEEK );
		if( nAvail <= 0 )
		{
			ErrorMsg( "Etherlink::LinkRead() : Socket closed" );
			CloseSocket();
			break;
		} /* if */

		/*
		 * Read in as much data as is available, or everything that has been requested
		 */
		if( nAvail > (int)uSize )
			nAvail = uSize;

		/*
		 * Now actually receive the data!
		 */
		nRead = recv( nSocket, pch, nAvail, 0 );
		if( nRead <= 0 )
		{
			ErrorMsg("Etherlink::LinkRead() : Unable to receive data from socket");
			CloseSocket();
			break;
		}

		pch += nRead;
		uCnt += nRead;
		uSize -= nRead;
	} /* while */

    return uCnt;
}

void CloseSocket(void)
{
	if( nSocket != INVALID_SOCKET )
	{
		close( nSocket );
		nSocket = INVALID_SOCKET;
	} /* if */
}

/***************************************************************************

Routine:           LinkClose

Inputs:
    None

Returns:
    void

Description:
    Closes the communications channel to the OPTOTRAK.

	Note: Reference counting was removed to fix 20362.
***************************************************************************/
NDI_DECL1 boolean NDI_DECL2 LinkClose( void )
{
	CloseSocket();
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
	fd_set
		dtRdFds,
		dtWrFds;
	struct timeval
		dtTV;

	/*
	 * Make sure the link has been opened.
	*/
	if( nSocket == INVALID_SOCKET )
	{
		return FALSE;
	}

	*pbReadable = FALSE;
	*pbWriteable = FALSE;

	FD_ZERO( &dtRdFds );
	FD_ZERO( &dtWrFds );

	FD_SET( nSocket, &dtRdFds );
	FD_SET( nSocket, &dtWrFds );

	dtTV.tv_sec = 0;
	dtTV.tv_usec = 1;

	if( select( nSocket + 1, &dtRdFds, &dtWrFds, 0, &dtTV ) < 0 )
	{
		ErrorMsg("Etherlink::LinkStatus() : Unable to determine socket's status");
		CloseSocket();
		return FALSE;
	} /* if */

	if( FD_ISSET( nSocket, &dtRdFds ) )
	{
		/* Peek at the data to see if there is any.
		   NOTE: The 'readable' flag means that there is either data availble or the socket was closed.
		*/
		char ch;
		int	nAvail = recv( nSocket, &ch, 1, MSG_PEEK );
		if ( nAvail != 1 )
		{
			ErrorMsg( "Etherlink::LinkStatus() : Socket closed" );
			CloseSocket();
			return FALSE;
		}
		*pbReadable = TRUE;
	}
	if( FD_ISSET( nSocket, &dtWrFds ) )
		*pbWriteable = TRUE;

    return TRUE;
}

/***************************************************************************

Routine:           bWaitForReply

Inputs:
    SOCKET	sock		: Socket from which data should be read
	char *pszBuff 		: Buffer to which data should be stored		
	unsigned uMaxLen	: Size of the buffer
	unsigned uTimeout	: Timeout (in seconds)

Returns:
    void

Description:
    Waits for a "carriage-return" terminated string.

***************************************************************************/
boolean bWaitForReply( int sock, char *pszBuff, unsigned uMaxLen, unsigned uTimeout )
{
	boolean
		bRet = FALSE,
		bDone = FALSE;
	fd_set
		dtWrFds;
	struct timeval
		tv;
	unsigned char
		uch;

	do {
		tv.tv_sec = uTimeout;
		tv.tv_usec = 0;

		FD_ZERO( &dtWrFds );
		FD_SET( sock, &dtWrFds );

		/*
		 * Wait for data.  If select returns anything but 1, there was either a
		 * timeout or error!
		 */
		if( select( sock + 1, 0, &dtWrFds, 0, &tv ) != 1 )
		{
			ErrorMsg( "Etherlink::bWaitForReply() : Unable to determine socket's status");
			CloseSocket();
			break;
		} /* if */

		/*
		 * Get the next byte from the socket!
		 */
		switch( recv( sock, &uch, 1, 0 ) )
		{
			case 0:		/* socket closed */
				bDone = TRUE;
				break;
			case 1:		/* a byte was received */
				*pszBuff++ = uch;
				uMaxLen--;

				/*
				 * if this character was a carriage return, we're done!
				 */
				if( uch == '\r' )
				{
					*pszBuff = '\0';
					bRet = TRUE;
					bDone = TRUE;
				} /* if */
				else if( uMaxLen <= 1 )
				{
					*pszBuff = '\0';
					bDone = TRUE;
				} /* if */
				break;
			default:	/* an error occured */
				bDone = TRUE;
				break;
		} /* switch */

	} while( !bDone );

	return bRet;
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
	struct sockaddr_in
		sockAddr;
	int
		nCtrlSocket = INVALID_SOCKET;
	boolean
		bRet = FALSE;
	char
		szReset[] = "RESET \r",
		szReply[ 128 ];

	/* NOTE: The link interface requires that LinkOpen be called before LinkReset
			this is not needed for the EtherLink implementation. But for consistency
			across the various implementations, we enforce this requirement.
	*/
	if( nSocket == INVALID_SOCKET )
	{
		ErrorMsg("EtherLink::LinkReset() : Unable to reset, link not open");
		return FALSE;
	}

	CloseSocket();

	Pause( 1. );
	nCtrlSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if( nCtrlSocket == INVALID_SOCKET )
	{
		ErrorMsg( "EtherLink::LinkReset() : Unable to create socket" );
		goto cleanup;
	} /* if */

	memset( &sockAddr, 0, sizeof(sockAddr) );

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr( szSrvrName );

	if( sockAddr.sin_addr.s_addr == -1 )
	{
		struct hostent *phost;
		phost = gethostbyname( szSrvrName );
		if( phost != NULL )
		{
			sockAddr.sin_addr.s_addr = ((struct in_addr *)phost->h_addr)->s_addr;
		}
		else
		{
			ErrorMsg( "EtherLink::LinkReset() : server '%s' could not be found", szSrvrName );
			goto cleanup; 
		} /* else */
	}

	sockAddr.sin_port = htons( (unsigned short)nCtrlPort );

	if( connect( nCtrlSocket, (struct sockaddr *)&sockAddr, sizeof(sockAddr) ) != 0 )
	{
		ErrorMsg( "EtherLink::LinkReset() : Unable to connect to server" );
		goto cleanup;
	} /* if */

	if( send( nCtrlSocket, szReset, strlen( szReset ), 0 ) < 0 )
	{
		ErrorMsg( "EtherLink::LinkReset() : Unable to Reset server" );
		goto cleanup;
	} /* if */

	if( !bWaitForReply( nCtrlSocket, szReply, sizeof(szReply), 2 ) )
	{
		ErrorMsg( "EtherLink::LinkReset() : Failed reseting server" );
		goto cleanup;
	} /* if */

	close( nCtrlSocket );
	nCtrlSocket = INVALID_SOCKET;

	/* reopen the socket.  We do this to more closely emulate PCI, which is "always open" */
	Pause( nResetDelay / 1000 ); /*  give the system some time to initialize */

	bRet = bOpenSocket();

cleanup:
	if( nCtrlSocket != INVALID_SOCKET )
	{
		close( nCtrlSocket );
	} /* if */

	return bRet;
}

