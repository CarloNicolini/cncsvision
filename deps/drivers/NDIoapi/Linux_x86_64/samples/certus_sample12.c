/*****************************************************************
Name:             CERTUS_SAMPLE12.C

Description:
 	Optotrak Certus Sample 12

	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set any available beepers on for 1 second.
	- Disconnect from the Optotrak System.

*****************************************************************/

/*****************************************************************
C Library Files Included
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <string.h>
void sleep( unsigned int uSec );
#endif

/*****************************************************************
ND Library Files Included
*****************************************************************/
#include "ndtypes.h"
#include "ndpack.h"
#include "ndopto.h"

#define SAMPLE_MARKERFREQ	2500.0f
#define SAMPLE_FRAMEFREQ	30.0f
#define SAMPLE_DUTYCYCLE	0.35f
#define SAMPLE_VOLTAGE		7.0f
#define SAMPLE_STREAMDATA	0

#define BEEPER_STATE_OFF	0
#define BEEPER_STATE_ON		1


/*****************************************************************
Application Files Included
*****************************************************************/
#include "certus_aux.h"


void main( int argc, unsigned char *argv[] )
{
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];
    int
		i,
		nDevices;
	ApplicationDeviceInformation
		*pdtDevices;
    DeviceHandle
        *pdtDeviceHandles;
    DeviceHandleInfo
        *pdtDeviceHandlesInfo;


    /*
     * initialization
	 * intialize variables
     */
	pdtDevices = NULL;
	pdtDeviceHandles = NULL;
	pdtDeviceHandlesInfo = NULL;
	nDevices = 0;

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak Certus sample program #12\n\n" );

	/*
	 * look for the -nodld parameter that indicates 'no download'
	 */
	if( ( argc < 2 ) || ( strncmp( argv[1], "-nodld", 6 ) != 0 ) )
	{
		/*
		 * Load the system of processors.
		 */
		fprintf( stdout, "...TransputerLoadSystem\n" );
		if( TransputerLoadSystem( "system" ) != OPTO_NO_ERROR_CODE )
		{
			goto ERROR_EXIT;
		} /* if */

		sleep( 1 );
	} /* if */

    /*
     * Communication Initialization
     * Once the system processors have been loaded, the application
     * prepares for communication by initializing the system processors.
     */
	fprintf( stdout, "...TransputerInitializeSystem\n" );
    if( TransputerInitializeSystem( OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	 * Determine if this sample will run with the system attached.
	 * This sample is intended for Optotrak Certus systems.
	 */
	fprintf( stdout, "...DetermineSystem\n" );
	if( uDetermineSystem( ) != OPTOTRAK_CERTUS_FLAG )
	{
		goto PROGRAM_COMPLETE;
	} /* if */

    /*
     * Strober Initialization
     * Once communication has been initialized, the application must
     * determine the strober configuration.
     * The application retrieves device handles and all strober
     * properties from the system.
     */
	fprintf( stdout, "...DetermineStroberConfiguration\n" );
	if( DetermineStroberConfiguration( &pdtDeviceHandles, &pdtDeviceHandlesInfo, &nDevices ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	 * check if any devices have been detected by the system
	 */
	if( nDevices == 0 )
	{
		fprintf( stdout, ".........no devices detected.  Quitting program...\n" );
		goto PROGRAM_COMPLETE;
	} /* if */

    /*
     * Now that all the device handles have been completely set up,
     * the application can store all the device handle information in
     * an internal data structure.  This will facilitate lookups when
     * a property setting needs to be checked.
     */
    ApplicationStoreDeviceProperties( &pdtDevices, pdtDeviceHandlesInfo, nDevices );


	/*
     * Turn the beeper ON
     */
	for( i = 0; i < nDevices; i++ )
	{
		if ( pdtDevices[i].nBeeper )
		{
			fprintf( stdout, "...OptotrakDeviceHandleSetBeeper (ON)\n" );
			if( OptotrakDeviceHandleSetBeeper( pdtDeviceHandlesInfo[i].pdtHandle->nID, BEEPER_STATE_ON ) != OPTO_NO_ERROR_CODE )
			{
				goto ERROR_EXIT;
			} /* if */

			/*
			 * Wait 1 second
			 */
			fprintf( stdout, "...beeping for one second\n" );
			sleep( 1 );

			/*
			 * Turn the beeper OFF
			 */
			fprintf( stdout, "...OptotrakDeviceHandleSetBeeper (OFF)\n" );
			if( OptotrakDeviceHandleSetBeeper( pdtDeviceHandlesInfo[i].pdtHandle->nID, BEEPER_STATE_OFF ) != OPTO_NO_ERROR_CODE )
			{
				goto ERROR_EXIT;
			} /* if */
		} /* if */
		else
		{
			fprintf( stdout, "...No beeper available on Device %.3d (ID %d)", (i + 1), pdtDeviceHandlesInfo[i].pdtHandle->nID );
		} /* else */
	} /* for */


PROGRAM_COMPLETE:
    /*
     * CLEANUP
     */
	fprintf( stdout, "\n" );
	fprintf( stdout, "...TransputerShutdownSystem\n" );
    OptotrakDeActivateMarkers( );
    TransputerShutdownSystem( );
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

	fprintf( stdout, "\n\n...TransputerShutdownSystem\n" );
	OptotrakDeActivateMarkers( );
	TransputerShutdownSystem( );

    exit( 1 );

} /* main */

