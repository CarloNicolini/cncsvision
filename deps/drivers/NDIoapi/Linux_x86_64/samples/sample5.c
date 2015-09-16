/*****************************************************************
Name:             SAMPLE5.C

Description:

    Optotrak Sample Program #5.

	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Initialize data file for spooling Raw Optotrak data.
	- Start spooling data to file.
	- Request/receive/display real-time 3D data while at the
	  same time writing spooled Raw data to file for 4 seconds.
	- De-activate the markers.
	- Disconnect from the Optotrak System.

*****************************************************************/

/*****************************************************************
C Library Files Included
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>

/*****************************************************************
ND Library Files Included
*****************************************************************/
#include "ndtypes.h"
#include "ndpack.h"
#include "ndopto.h"

#ifdef _MSC_VER
#include <string.h>
void sleep( unsigned int uSec );
#endif

/*****************************************************************
Application Files Included
*****************************************************************/
#include "ot_aux.h"

/*****************************************************************
Defines:
*****************************************************************/

#define NUM_MARKERS     6

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
        uMarkerCnt,
        uFrameNumber,
        uSpoolStatus,
        uSpoolComplete,
        uRealtimeDataReady;
    static Position3d
        p3dData[NUM_MARKERS];
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #5\n\n" );

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
            NUM_MARKERS,    /* Number of markers in the collection. */
            (float)50.0,    /* Frequency to collect data frames at. */
            (float)2500.0,  /* Marker frequency for marker maximum on-time. */
            30,             /* Dynamic or Static Threshold value to use. */
            160,            /* Minimum gain code amplification to use. */
            1,              /* Stream mode for the data buffers. */
            (float)0.4,     /* Marker Duty Cycle to use. */
            (float)7.5,     /* Voltage to use when turning on markers. */
            (float)4.0,     /* Number of seconds of data to collect. */
            (float)0.0,     /* Number of seconds to pre-trigger data by. */
            OPTOTRAK_NO_FIRE_MARKERS_FLAG | OPTOTRAK_BUFFER_RAW_FLAG | OPTOTRAK_GET_NEXT_FRAME_FLAG ) )
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
     * Initialize the necessary spooling variables and a file for spooling
     * of the Optotrak data.
     */
    uSpoolStatus       =
    uSpoolComplete     =
    uRealtimeDataReady = 0;
	fprintf( stdout, "...DataBufferInitializeFile\n" );
    if( DataBufferInitializeFile( OPTOTRAK, "R#001.S05" ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Start the Optotrak spooling data to us.
     */
	fprintf( stdout, "...DataBufferStart\n" );
    if( DataBufferStart() )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Request a frame of realtime 3D data.
     */
    if( RequestLatest3D() )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Loop around spooling data to file and displaying realtime 3d data.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    do
    {
        /*
         * Write data if there is any to write.
         */
        if( DataBufferWriteData( &uRealtimeDataReady, &uSpoolComplete,
                                 &uSpoolStatus, NULL ) )
        {
            goto ERROR_EXIT;
        } /* if */

        /*
         * Display realtime if there is any to display.
         */
        if( uRealtimeDataReady )
        {
            /*
             * Receive the 3D data.
             */
            if( DataReceiveLatest3D( &uFrameNumber, &uElements, &uFlags,
                                     p3dData ) )
            {
                goto ERROR_EXIT;
            } /* if */

            /*
             * Print out 3D data.
             */
            fprintf( stdout, "Frame Number: %8u\n", uFrameNumber );
            fprintf( stdout, "Elements    : %8u\n", uElements );
            fprintf( stdout, "Flags       : 0x%04x\n", uFlags );
            for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; ++uMarkerCnt )
            {
				DisplayMarker( uMarkerCnt + 1, p3dData[uMarkerCnt] );
            } /* for */

            /*
             * Request a new frame of realtime 3D data.
             */
            if( RequestLatest3D() )
            {
                goto ERROR_EXIT;
            } /* if */
        } /* if */

    } /* do */
    while( !uSpoolComplete );

    fprintf( stdout, "Spool Status: 0x%04x\n", uSpoolStatus );
    fprintf( stdout, "\n" );

	while( !DataIsReady( ) );

	if( DataReceiveLatest3D( &uFrameNumber, &uElements, &uFlags, p3dData ) )
	{
		goto ERROR_EXIT;
	} /* if */

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
