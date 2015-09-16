/*****************************************************************
Name:             SAMPLE17.C

Description:

    Optotrak Sample Program #17.

	- Initiate communications with the Optotrak System.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up the strober port table.
	- Set up an Optotrak collection.
	- Initialize data file for spooling 3D data.
	- Request/receive real-time 3D data until Marker_01 appears.
	- Start spooling data to file.
	- Request/receive real-time 3D data while at the same time writing
	  spooled 3D data to file.
	- Stop spooling to file after 5 seconds of data, or when
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

#define NUM_MARKERS	6

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
    unsigned int
        uFlags,
        uElements,
        uFrameNumber,
        uSpoolStatus,
        uSpoolComplete,
        uRealtimeDataReady;
	unsigned long
		ulFramesBuffered;
    Position3d
        p3dData[NUM_MARKERS];
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #17\n\n" );

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
     * Set optional processing flags (this overides the settings in Optotrak.INI).
     */
	fprintf( stdout, "...OptotrakSetProcessingFlags\n" );
    if( OptotrakSetProcessingFlags( OPTO_LIB_POLL_REAL_DATA |
                                    OPTO_CONVERT_ON_HOST |
                                    OPTO_RIGID_ON_HOST ) )
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

	if( OptotrakSetStroberPortTable( NUM_MARKERS, 0, 0, 0 ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */

    /*
     * Set up a collection for the Optotrak.
     */
	fprintf( stdout, "...OptotrakSetupCollection\n" );
    if( OptotrakSetupCollection(
            NUM_MARKERS,    /* Number of markers in the collection. */
            (float)10.0,   /* Frequency to collect data frames at. */
            (float)2500.0,  /* Marker frequency for marker maximum on-time. */
            30,             /* Dynamic or Static Threshold value to use. */
            160,            /* Minimum gain code amplification to use. */
            0,              /* Stream mode for the data buffers. */
            (float)0.35,    /* Marker Duty Cycle to use. */
            (float)7.0,     /* Voltage to use when turning on markers. */
            (float)5.0,    /* Number of seconds of data to collect. */
            (float)0.0,     /* Number of seconds to pre-trigger data by. */
            OPTOTRAK_NO_FIRE_MARKERS_FLAG | OPTOTRAK_GET_NEXT_FRAME_FLAG ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Wait one second to let the camera adjust.
     */
    sleep( 1 );

    /*
     * Initialize a file for spooling of the Optotrak 3D data.
     */
	fprintf( stdout, "...DataBufferInitializeFile\n" );
    if( DataBufferInitializeFile( OPTOTRAK, "C#001.S17" ) )
    {
        goto ERROR_EXIT;
    } /* if */

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
     * Loop until marker 1 comes into view.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
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
    while( p3dData[0].x < MAX_NEGATIVE );

    /*
     * Initialize the necessary spooling variables and a file for spooling
     * of the Optotrak data.
     */
    uSpoolStatus       =
    uSpoolComplete     =
    uRealtimeDataReady = 0;

    /*
     * Start the Optotrak spooling data to us.
     */
	fprintf( stdout, "\nDataBufferStart...\n" );
	if( DataBufferStart( ) )
	{
		goto ERROR_EXIT;
	} /* if */

    /*
     * Loop around spooling data to file until marker 1 goes out of view.
     */
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
		if( p3dData[0].x < MAX_NEGATIVE )
		{
			if( DataBufferStop() )
			{
				goto ERROR_EXIT;
			} /* if */
		} /* if */

        /*
         * Write data if there is any to write.
         */
		if( DataBufferWriteData( &uRealtimeDataReady, &uSpoolComplete, &uSpoolStatus, &ulFramesBuffered ) )
		{
			goto ERROR_EXIT;
		} /* if */

		fprintf( stdout, "FramesBuffered: %d\n", ulFramesBuffered );
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
