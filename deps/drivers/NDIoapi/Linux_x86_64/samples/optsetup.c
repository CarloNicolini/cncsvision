/*****************************************************************
Name:             OPTSETUP.C

Description:
	Optotrak Optsetup
	Perform function call to determine the processor configuration.
	This is the equivalent of the old 'optsetup' program.

	- Determine System Configuration

*****************************************************************/

/*****************************************************************
C Library Files Included
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
void sleep( unsigned int uSec );
#endif

/*****************************************************************
ND Library Files Included
*****************************************************************/
#include "ndtypes.h"
#include "ndpack.h"
#include "ndopto.h"


/*****************************************************************
Application Files Included
*****************************************************************/

#define NETWORK_INFORMATION_FILE	"/realtime/system.nif"

void main( int argc, unsigned char *argv[] )
{
	char
		*pszNDDIR,
		szNetworkInformationFile[_MAX_FNAME],
		szLine[64],
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];
	FILE
		*fileNIF;

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak Setup\n\n" );

	/*
	 * determine the location of the Optotrak NIF file
	 */
	fprintf( stdout, "...reading ND_USER_DIR: " );
	pszNDDIR = getenv( "ND_USER_DIR" );
	if( !pszNDDIR )
	{
		fprintf( stdout, "\n...reading ND_DIR: " );
		pszNDDIR = getenv( "ND_DIR" );
	} /* if */
	if( !pszNDDIR )
	{
		fprintf( stdout, "\n\nAn error has occurred during execution of the program.\n" );
		fprintf( stdout, "ND_DIR environment variable has not been set.\n" );
	} /* if */
	fprintf( stdout, "%s\n\n", pszNDDIR );

	sprintf( szNetworkInformationFile, "%s%s", pszNDDIR, NETWORK_INFORMATION_FILE );
	fprintf( stdout, "Optotrak Network InformationFile: %s\n\n", szNetworkInformationFile );

	/*
	 * delete the old NIF file
	 */
	unlink( szNetworkInformationFile );

    /*
     * System Initialization
     * During initialization, the application first determines the system
     * configuration of processors (SCU, Position Sensors, ODAU) then
     * loads the appropriate system code to the processors.
     * After initialization, the application can communicate with the system.
     */
	fprintf( stdout, "...TransputerDetermineSystemCfg\n" );
    if( TransputerDetermineSystemCfg( NULL ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	 * open the new NIF file
	 */
	fileNIF = fopen( szNetworkInformationFile, "r" );
	if( !fileNIF )
	{
		fprintf( stdout, "Unable to open Network Information File (%s)", szNetworkInformationFile );
		goto ERROR_EXIT;
	} /* if */

	/*
	 * print the contents of the new NIF file
	 */
	fprintf( stdout, "\nNetwork Information:\n" );
	while( fgets( szLine, sizeof( szLine ), fileNIF ) )
	{
		fprintf( stdout, "%s", szLine );
	} /* while */
	fprintf( stdout, "\n" );

	/*
	 * close the new NIF file
	 */
	fclose( fileNIF );

	exit( 0 );

ERROR_EXIT:
	/*
	 * Indicate that an error has occurred
	 */
	fprintf( stdout, "\nAn error has occurred during execution of the program.\n" );
    if( OptotrakGetErrorString( szNDErrorString, MAX_ERROR_STRING_LENGTH + 1 ) == 0 )
    {
        fprintf( stdout, szNDErrorString );
    } /* if */
    exit( 1 );

} /* main */

