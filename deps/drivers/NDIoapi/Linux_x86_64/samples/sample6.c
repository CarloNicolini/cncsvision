/*****************************************************************
Name:             SAMPLE6.C

Description:

    Optotrak Sample Program #6.

	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Initialize a memory block for spooling 3D data.
	- Start spooling data to memory.
	- Request/receive/display real-time 3D data while at the
	  same time writing spooled 3D data to file for 5 seconds.
	- De-activate the markers.
	- Display the 250 frames of data stored in the memory block.
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
#define FRAME_RATE      (float)50.0
#define COLLECTION_TIME (float)5.0

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
        uFrameCnt,
        uMarkerCnt,
        uFrameNumber,
        uSpoolStatus,
        uSpoolComplete,
        uMemoryRequired,
        uRealtimeDataReady;
    unsigned char
        *puchTempPtr    = NULL;
    static Position3d
        p3dTemp,
        *p3dTempPtr    = NULL,
        *p3dDataBuffer = NULL,
        p3dData[ NUM_MARKERS];
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #6\n\n" );

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
            (float)0.35,        /* Marker Duty Cycle to use. */
            (float)7.0,         /* Voltage to use when turning on markers. */
            COLLECTION_TIME,    /* Number of seconds of data to collect. */
            (float)0.0,         /* Number of seconds to pre-trigger data by. */
            OPTOTRAK_NO_FIRE_MARKERS_FLAG | OPTOTRAK_GET_NEXT_FRAME_FLAG ) )
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
     * Allocate enough memory to store the Optotrak 3D data.
     */
    uMemoryRequired = (unsigned int)( COLLECTION_TIME * FRAME_RATE *
                                      NUM_MARKERS * sizeof( Position3d ) );
    p3dDataBuffer = (Position3d *)malloc( uMemoryRequired );
    if( NULL == p3dDataBuffer )
    {
        fprintf( stderr, "Error: Unable to allocate memory.\n" );
        goto ERROR_EXIT;
    } /* if */

    /*
     * Initialize an area of memory for spooling of the Optotrak data.
     */
	fprintf( stdout, "...DataBufferInitializeMem\n" );
    if( DataBufferInitializeMem( OPTOTRAK, p3dDataBuffer ) )
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
     * Start the Optotrak spooling data to us.
     */
	fprintf( stdout, "...DataBufferStart\n" );
    if( DataBufferStart() )
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
         * Get a frame of 3D data.
         */
        if( DataGetLatest3D( &uFrameNumber, &uElements, &uFlags, p3dData ) )
        {
            goto ERROR_EXIT;
        } /* if */

        /*
         * Print out the data.
         */
        fprintf( stdout, "Frame Number: %8u\n", uFrameNumber );
        fprintf( stdout, "Elements    : %8u\n", uElements );
        fprintf( stdout, "Flags       : 0x%04x\n", uFlags );
        for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; ++uMarkerCnt )
        {
			DisplayMarker( uMarkerCnt + 1, p3dData[uMarkerCnt] );
        } /* for */

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
     * Print out the data spooled to the 3D buffer.
     */
    p3dTempPtr = p3dDataBuffer;
    for( uFrameCnt = 0; uFrameCnt < COLLECTION_TIME * FRAME_RATE; ++uFrameCnt )
    {
        /*
         * Print out the current frame.
         */
        fprintf( stdout, "\nFrame %u\n", uFrameCnt + 1 );

        /*
         * Print the data for each marker in this frame.
         */
        for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; ++uMarkerCnt )
        {
            p3dTemp = *p3dTempPtr++;
			DisplayMarker( uMarkerCnt + 1, p3dTemp );
        } /* for */
    } /* for */
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
    if( NULL != p3dDataBuffer )
    {
        free( p3dDataBuffer );
    } /* if */
    fprintf( stdout, "\nProgram execution complete.\n" );
    exit( 0 );

ERROR_EXIT:
	/*
	 * Indicate that an error has occurred
	 */
	fprintf( stdout, "\nAn error has occurred during execution of the program.\n" );
    if( NULL != p3dDataBuffer )
    {
        free( p3dDataBuffer );
    } /* if */
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
