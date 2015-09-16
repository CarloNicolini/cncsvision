/*****************************************************************
Name:             SAMPLE1.C

Description:

    Optotrak Sample Program #1.

	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Request/receive/display the current Optotrak System status.
	- Disconnect from the Optotrak System.

*****************************************************************/

/*****************************************************************
C Library Files Included
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
Name:               main

Input Values:
    int
        argc        :Number of command line parameters.
    unsigned char
        *argv[]     :Pointer array to each parameter.

Output Values:
    None.

Return Value:
    None.

Description:

    Main program routine performs all steps listed in the above
    program description.

*****************************************************************/
void main( int argc, unsigned char *argv[] )
{
    int
        nNumSensors,
        nNumOdaus,
        nMarkers;
    char
		szOAPIVersion[64],
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #1\n\n" );

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
     * Wait one second to let the system finish loading.
     */
    sleep( 1 );

    /*
     * Initialize the processors system.
     */
	fprintf( stdout, "...TransputerInitializeSystem\n" );
    if( TransputerInitializeSystem( OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG ))
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Load the standard camera parameters.
     */
	fprintf( stdout, "...OptotrakLoadCameraParameters\n" );
    if( OptotrakLoadCameraParameters( "standard" ) )
    {
        goto ERROR_EXIT;
    } /* if */

	/*
	 * Retrieve the OAPI version string
	 */
	fprintf( stdout, "...OAPIGetVersionString\n" );
	if( OAPIGetVersionString( szOAPIVersion, sizeof( szOAPIVersion) ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */
	fprintf( stdout, "\t%s\n", szOAPIVersion );

    /*
     * Request and receive the Optotrak status.
     */
	fprintf( stdout, "...OptotrakGetStatus\n" );
    if( OptotrakGetStatus(
            &nNumSensors,    /* Number of sensors in the Optotrak system. */
            &nNumOdaus,      /* Number of ODAUs in the Optotrak system. */
            NULL,            /* Number of rigid bodies being tracked by the O/T. */
            &nMarkers,       /* Number of markers in the collection. */
            NULL,            /* Frequency that data frames are being collected. */
            NULL,            /* Marker firing frequency. */
            NULL,            /* Dynamic or Static Threshold value being used. */
            NULL,            /* Minimum gain code amplification being used. */
            NULL,            /* Stream mode indication for the data buffers */
            NULL,            /* Marker Duty Cycle being used. */
            NULL,            /* Voltage being used when turning on markers. */
            NULL,            /* Number of seconds data is being collected. */
            NULL,            /* Number of seconds data is being pre-triggered. */
            NULL ) )         /* Configuration flags. */
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Display elements of the status received.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    fprintf( stdout, "Sensors in system       :%3d\n", nNumSensors );
    fprintf( stdout, "ODAUs in system         :%3d\n", nNumOdaus );
    fprintf( stdout, "Default Optotrak Markers:%3d\n", nMarkers );
    fprintf( stdout, "\n" );

    /*
     * Shutdown the processors message passing system.
     */
	fprintf( stdout, "...TransputerShutdownSystem\n" );
    if( TransputerShutdownSystem() )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Exit the program.
     */
    fprintf( stdout, "\nProgram execution complete.\n" );
    exit( 0 );

ERROR_EXIT:
	/*
	 * Indicate that an error has occurred
	 */
	fprintf( stdout, "\nAn error has occurred during execution of the program.\n" );
    if( OptotrakGetErrorString( szNDErrorString,
                                MAX_ERROR_STRING_LENGTH + 1 ) == 0 )
    {
        fprintf( stdout, szNDErrorString );
    } /* if */

	fprintf( stdout, "\n\n...TransputerShutdownSystem\n" );
    TransputerShutdownSystem();
    exit( 1 );

} /* main */
