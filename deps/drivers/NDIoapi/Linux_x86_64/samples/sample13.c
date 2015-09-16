/*****************************************************************
Name:             SAMPLE13.C

Description:

    Optotrak Sample Program #13.

	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an ODAU collection.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Initialize data file for spooling 3D data.
	- Initialize data file for spooling ODAU data.
	- Collect and spool 2 seconds of 3D and ODAU data to file.
	- De-activate the markers.
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

/*****************************************************************
Defines:
*****************************************************************/

#define NUM_SENSORS     3
#define NUM_MARKERS     6
#define NUM_CHANNELS    8
#define ODAU_GAIN       1

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
    unsigned int
        uSpoolStatus = 0;
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #13\n\n" );

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
    if( TransputerInitializeSystem( OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG ) )
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
     * Set up a collection for the ODAU.
     */
	fprintf( stdout, "...OdauSetupCollection\n" );
    if( OdauSetupCollection(
            ODAU1,                  /* Id the ODAU the parameters are for. */
            NUM_CHANNELS,           /* Number of analog channels to collect. */
            ODAU_GAIN,              /* Gain to use for the analog channels.*/
            ODAU_DIGITAL_INPB_INPA, /* Mode for the Digital I/O port.*/
            (float)100.0,           /* Frequency to collect data frames at. */
            (float)90000.0,         /* Frequency to scan channels at. */
            1,                      /* Stream mode for the data buffers. */
            (float)2.0,             /* Number of seconds of data to collect. */
            (float)0.0,             /* Number of seconds to pre-trigger data by. */
            0 ) )                   /* Flags. */
    {
    goto ERROR_EXIT;
    } /* if */

    /*
     * Set up a collection for the Optotrak.
     */
	fprintf( stdout, "...OptotrakSetupCollection\n" );
    if( OptotrakSetupCollection(
            NUM_MARKERS,    /* Number of markers in the collection. */
            (float)50.0,    /* Frequency to collect data frames at. */
            (float)2500.0,  /* Marker frequency for marker maximum on-time. */
            30,             /* Dynamic or Static Threshold value to use. */
            160,            /* Minimum gain code amplification to use. */
            1,              /* Stream mode for the data buffers. */
            (float)0.40,    /* Marker Duty Cycle to use. */
            (float)8.0,     /* Voltage to use when turning on markers. */
            (float)2.0,     /* Number of seconds of data to collect. */
            (float)0.0,     /* Number of seconds to pre-trigger data by. */
            OPTOTRAK_NO_FIRE_MARKERS_FLAG ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Wait one second to let the camera adjust.
     */
    sleep( 1 );

    /*
     * Activate the markers.
     */
	fprintf( stdout, "...OptotrakActivateMarkers\n" );
    if( OptotrakActivateMarkers() )
    {
        goto ERROR_EXIT;
    } /* if */
	sleep( 1 );

    /*
     * Initialize a file for spooling of the Optotrak 3D data.
     */
	fprintf( stdout, "...DataBufferInitializeFile\n" );
    if( DataBufferInitializeFile( OPTOTRAK, "C#001.S13" ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Initialize a file for spooling of the ODAU raw data.
     */
	fprintf( stdout, "...DataBufferInitializeFile\n" );
    if( DataBufferInitializeFile( ODAU1, "O1#001.S13" ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Spool data to the previously initialized files.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    fprintf( stdout, "Collecting data files...\n" );
    if( DataBufferSpoolData( &uSpoolStatus ) )
    {
        goto ERROR_EXIT;
    } /* if */
    fprintf( stdout, "Spool Status: 0x%04x\n", uSpoolStatus );
    fprintf( stdout, "\n" );

    /*
     * De-activate the markers.
     */
	fprintf( stdout, "...OptotrakDeActivateMarkers\n" );
    if( OptotrakDeActivateMarkers() )
    {
        goto ERROR_EXIT;
    } /* if */

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
    OptotrakDeActivateMarkers();
    TransputerShutdownSystem();

    exit( 1 );

} /* main */
