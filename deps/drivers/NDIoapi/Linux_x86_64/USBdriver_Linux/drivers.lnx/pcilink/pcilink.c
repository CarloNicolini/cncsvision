/*
 *  Optotrak/PCI API interface driver for Linux
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "optopci.h"
#include "ndtypes.h"
#include "ndpack.h"
#include "ndopto.h"

static int fdPCI;

/***************************************************************************

Routine:    LinkOpen

Inputs:     None

Returns:    "boolean"  (TRUE if successful)

Description: Opens the /dev/optopci device for communication to the OPTOTRAK

***************************************************************************/
boolean LinkOpen(void)
{
	

	if( (fdPCI = open( "/dev/optopci", O_RDWR )) < 0 )
		return FALSE;

	if(ioctl(fdPCI, OPTOPCI_CARD_INIT, 0) < 0) {
		close(fdPCI);
		return FALSE;
	}	

	return TRUE;
}

/***************************************************************************

Routine:      LinkClose

Inputs:       None

Returns:      "boolean"             : return FALSE if an error occurs.

Description:  Closes the /dev/optopci driver to the OPTOTRAK

***************************************************************************/
boolean LinkClose(void)
{
	if (fdPCI >= 0) {
		close(fdPCI);
		fdPCI = -1;
	}
	return TRUE;
}

/***************************************************************************

Routine:    LinkRead

Inputs:     void *pData           : Buffer for data
            unsigned uBytes       : Number of bytes to read

Returns:    unsigned              : Number of bytes actually read

Description:
    LinkRead attempts to rececive "uBytes" of data from the OPTOTRAK.
    The data is read into the specified buffer.

***************************************************************************/

unsigned LinkRead(void *pData, unsigned uBytes)
{
	int
		nTotRead = 0,
		nRead;
	unsigned char
		*puchData = (unsigned char *)pData;	

	if (fdPCI < 0)
		return FALSE;

	do {
		nRead = read(fdPCI, puchData, uBytes);
		if( nRead < 0 )
		{			
			return 0;
		} 

		uBytes -= nRead;
		puchData += nRead;
		nTotRead += nRead;
	} while( uBytes );		

	return nTotRead;
}

/***************************************************************************

Routine:           LinkWrite

Inputs:
    void *pData           : Data buffer to be written to the link
    unsigned uBytes       : Number of bytes to write

Returns:
    unsigned              : Number of bytes actually written.

Description:
    LinkWrite attempts to send a block of "uBytes" of data to the
    OPTOTRAK.

***************************************************************************/
unsigned LinkWrite(void *pData, unsigned uBytes)
{
	int
		nTotWritten = 0,
		nWritten;
	unsigned char
		*puchData = (unsigned char *)pData;	

	if (fdPCI < 0)
		return FALSE;

	do {
		nWritten = write(fdPCI, puchData, uBytes);
		if( nWritten < 0 )
		{			
			return 0;
		}

		uBytes -= nWritten;
		puchData += nWritten;
		nTotWritten += nWritten;
	} while( uBytes );
		

	return nTotWritten;
}

/***************************************************************************

Routine:           LinkStatus

Inputs:
    boolean *pbReadable   : Pointer to readable flag
    boolean *pbWriteable  : Pointer to writeable flag

Returns:
    boolean               : Returns FALSE if an error occurs

Description:
    LinkStatus determines whether or not there is any data available for 
	the host computer, and if so sets *pbReadable to TRUE (else FALSE)

    It also determines if the OPTOTRAK is ready to receive more data,
    and if so, sets *pbWriteable to TRUE (else FALSE).

***************************************************************************/
boolean LinkStatus(boolean * pbReadable, boolean * pbWriteable)
{
	unsigned long i;	

	if (fdPCI < 0)
		return FALSE;

	if (ioctl(fdPCI, OPTOPCI_LINK_STAT, &i) < 0) {
		*pbWriteable = *pbReadable = FALSE;
		return FALSE;
	}

	*pbWriteable = (i & OPTOTRAK_WRITE) ? TRUE : FALSE;
	*pbReadable = (i & OPTOTRAK_READ) ? TRUE : FALSE;

	return TRUE;

}

/***************************************************************************

Routine:           LinkReset

Inputs:
    None

Returns:
    void

Description:
    LinkReset performs a RESET of the OPTOTRAK system.  The LinkReset
    routine will prepare the OPTOTRAK for downloading.

***************************************************************************/
boolean LinkReset(void)
{	

	if (fdPCI < 0)
		return FALSE;

	if (ioctl(fdPCI, OPTOPCI_LINK_RESET, 0))
		return FALSE;

	sleep(1);

	if (ioctl(fdPCI, OPTOPCI_CARD_INIT, 0))
		return FALSE;

	sleep(1);

	return TRUE;
}
