/*****************************************************************
Name:             SAMPLE7.C

Description:

    Optotrak Sample Program #7.

	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Initialize data file for spooling 3D data.
	- Request/receive real-time 3D data until Marker_01 appears.
	- Start spooling data to file.
	- Request/receive/display real-time 3D data while at the
	  same time writing spooled 3D data to file.
	- Stop spooling to file after 100 seconds of data, or when
	  Marker_01 goes out of view.
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

#define NUM_MARKERS     6
#define FRAME_RATE      (float)50.0
#define COLLECTION_TIME (float)100.0

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
        uFlags,
        uElements,
        uFrameNumber,
        uSpoolStatus,
        uSpoolComplete,
        uRealtimeDataReady;
    static Position3d
        p3dData[ NUM_MARKERS];
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];
	boolean
		bBufferingStopped;

	/*
	 * initialize variables
	 */
	bBufferingStopped = FALSE;

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #7\n\n" );

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
     * Set up a collection for the Optotrak.
     */
	fprintf( stdout, "...OptotrakSetupCollection\n" );
    if( OptotrakSetupCollection(
            NUM_MARKERS,        /* Number of markers in the collection. */
            FRAME_RATE,         /* Frequency to collect data frames at. */
            (float)2500.0,      /* Marker frequency for marker maximum on-time. */
            30,                 /* Dynamic or Static Threshold value to use. */
            160,                /* Minimum gain code amplification to use. */
            1,                  /* Stream mode for the data buffers. */
            (float)0.4,         /* Marker Duty Cycle to use. */
            (float)7.5,         /* Voltage to use when turning on markers. */
            COLLECTION_TIME,    /* Number of seconds of data to collect. */
            (float)0.0,         /* Number of seconds to pre-trigger data by. */
            OPTOTRAK_NO_FIRE_MARKERS_FLAG ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Wait one second to let the collection finish setting up.
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
    if( DataBufferInitializeFile( OPTOTRAK, "C#001.S07" ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Initialize the necessary spooling variables and a file for spooling
     * of the Optotrak data.
     */
    uSpoolStatus       =
    uSpoolComplete     =
    uRealtimeDataReady = 0;

    /*
     * Loop until marker 1 comes into view.
     */
    fprintf( stdout, "Waiting for marker 1...\n" );
    do
    {
        /*
         * Get a frame of 3D data.
         */
        if( DataGetLatest3D( &uFrameNumber, &uElements, &uFlags, p3dData ) )
        {
            goto ERROR_EXIT;
        } /* if */
    } /* do */
    while( p3dData[ 0].x < MAX_NEGATIVE );

    /*
     * Start the Optotrak spooling data to us.
     */
	fprintf( stdout, "...DataBufferStart\n" );
    if( DataBufferStart() )
    {
        goto ERROR_EXIT;
    } /* if */
    fprintf( stdout, "Collecting data file...\n" );

    /*
     * Loop around spooling data to file until marker 1 goes out of view.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    do
    {
        /*
         * Get a frame of 3D data.
         */
        if( DataGetLatest3D( &uFrameNumber, &uElements, &uFlags, p3dData ) )
        {
            goto ERROR_EXIT;
        } /* if */

        /*
         * Check to see if marker 1 is out of view and stop the Optotrak from
         * spooling data if this is the case.
         */
        if( p3dData[ 0].x < MAX_NEGATIVE )
        {
			if( !bBufferingStopped )
			{
				if( DataBufferStop() )
				{
					goto ERROR_EXIT;
				} /* if */

				bBufferingStopped = TRUE;
			} /* if */
        } /* if */

        /*
         * Write data if there is any to write.
         */
        if( DataBufferWriteData( &uRealtimeDataReady, &uSpoolComplete,
                                 &uSpoolStatus, NULL ) )
        {
            goto ERROR_EXIT;
        } /* if */
    } /* do */
    while( !uSpoolComplete );

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
