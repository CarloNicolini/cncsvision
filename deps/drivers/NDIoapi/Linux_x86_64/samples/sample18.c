/*****************************************************************
Name:             SAMPLE18.C

Description:

    Optotrak Sample Program #18.

	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive 50 frames of 3D data.
	- De-activate the markers.
	- Average all valid frames of data.
	- Create a new coordinate system defined by the averaged positions.
	- Change the measurement coordinate system.
	- Activate the markers.
	- Request/receive/display 10 frames of 3D data.
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
Application Files Included
*****************************************************************/
#include "ot_aux.h"

/*****************************************************************
Defines:
*****************************************************************/

#define NUM_SENSORS 3
#define NUM_MARKERS 6

/*****************************************************************
Global Variables:
*****************************************************************/

/*
 * Arrays used in transforming the measurement coordinate system of the
 * Optotrak. The aligned positions are the marker positions relative to
 * marker 1 on a 6 marker rigid body probe.
 */
static Position3d
    grdt3dErrors[NUM_MARKERS],
    grdtAlignedPositions[NUM_MARKERS] =
    {
        { (float)0,      (float)0,     (float)0 },
        { (float)6.29,   (float)29.48, (float)9.8 },
        { (float)-27.64, (float)27.33, (float)38.19 },
        { (float)-61.86, (float)25.14, (float)66.58 },
        { (float)-68.08, (float)-4.39, (float)57.05 },
        { (float)-33.99, (float)-2.26, (float)28.58 }
    };

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
    float
        fRmsError;
    boolean
        bValid = TRUE;
    unsigned int
        uFlags,
        uElements,
		uFramesToAvg,
        uFrameCnt,
        uMarkerCnt,
        uFrameNumber;
    static Position3d
        grdt3dData[NUM_MARKERS],
        grdtAveraged3dData[NUM_MARKERS];
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];

    /*
     * Initialize the averaged marker position array to zeros.
     */
    for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; uMarkerCnt++ )
    {
        grdtAveraged3dData[uMarkerCnt].x =
        grdtAveraged3dData[uMarkerCnt].y =
        grdtAveraged3dData[uMarkerCnt].z = (float) 0.0;
    } /* for */

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #18\n\n" );

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
     * Set up a collection for the Optotrak.
     */
	fprintf( stdout, "...OptotrakSetupCollection\n" );
    if( OptotrakSetupCollection(
            NUM_MARKERS,    /* Number of markers in the collection. */
            (float)100.0,   /* Frequency to collect data frames at. */
            (float)2500.0,  /* Marker frequency for marker maximum on-time. */
            30,             /* Dynamic or Static Threshold value to use. */
            160,            /* Minimum gain code amplification to use. */
            0,              /* Stream mode for the data buffers. */
            (float)0.35,    /* Marker Duty Cycle to use. */
            (float)7.0,     /* Voltage to use when turning on markers. */
            (float)1.0,     /* Number of seconds of data to collect. */
            (float)0.0,     /* Number of seconds to pre-trigger data by. */
            OPTOTRAK_NO_FIRE_MARKERS_FLAG | OPTOTRAK_BUFFER_RAW_FLAG ) )
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
     * Average 50 frames of 3D data to a single frame.
     */
    uFrameCnt = 0;
	uFramesToAvg = 0;
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    fprintf( stdout, "\n\nAveraging 3D data...\n" );
    do
    {
        /*
         * Get a frame of data.
         */
        if( DataGetLatest3D( &uFrameNumber, &uElements, &uFlags, grdt3dData ) )
        {
            goto ERROR_EXIT;
        } /* if */

        /*
         * Determine if the frame is valid; that is if each marker has a 3D
         * coordinate.
         */
        bValid = TRUE;
        for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; uMarkerCnt++ )
        {
            if( grdt3dData[uMarkerCnt].x < MAX_NEGATIVE )
            {
                bValid = FALSE;
            } /* if */
        } /* for */

        /*
         * Add in each markers contribution if this frame was valid.
         */
        if( bValid )
        {
            for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; uMarkerCnt++ )
            {
                grdtAveraged3dData[uMarkerCnt].x += grdt3dData[uMarkerCnt].x;
                grdtAveraged3dData[uMarkerCnt].y += grdt3dData[uMarkerCnt].y;
                grdtAveraged3dData[uMarkerCnt].z += grdt3dData[uMarkerCnt].z;
				uFramesToAvg++;
            } /* for */
            ++uFrameCnt;
        } /* if */
    } /* do */
    while( uFrameCnt < 50 );

    /*
     * De-activate the markers.
     */
	fprintf( stdout, "...OptotrakDeActivateMarkers\n" );
    if( OptotrakDeActivateMarkers() )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Divide sums by 50 to get the averaged marker positions.
     */
    for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; uMarkerCnt++ )
    {
        grdtAveraged3dData[uMarkerCnt].x /= uFramesToAvg;
        grdtAveraged3dData[uMarkerCnt].y /= uFramesToAvg;
        grdtAveraged3dData[uMarkerCnt].z /= uFramesToAvg;
    } /* for */

    /*
     * Transform the measurement frame of reference for the Optotrak system
     * to the frame of reference defined by the grdtAlignedPositions array.
     */
    if( OptotrakChangeCameraFOR(
            "standard",             /* Camera Parameter File to load transform. */
            NUM_MARKERS,            /* Number of markers in 3D arrays. */
            grdtAlignedPositions,   /* Marker positions in current FOR. */
            grdtAveraged3dData,     /* Marker positions in desired FOR. */
            "new",                  /* Name of new camera parameter file. */
            grdt3dErrors,           /* 3D errors of alignment transformation. */
            &fRmsError ) )          /* RMS distance error of alignment transformation.*/
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Print out the results of the alignment.
     */
    fprintf( stdout, "RMS Fit Error : %6.3f\n", fRmsError );
    fprintf( stdout, "Coordinate Fit Error Table\n" );
    fprintf( stdout, "Mkr %6s %6s %6s\n","X", "Y", "Z" );
    for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; ++uMarkerCnt )
    {
		DisplayMarker( uMarkerCnt + 1, grdt3dErrors[uMarkerCnt] );
    } /* for */

    /*
     * Load the NEW camera parameter file to the Optotrak system.
     */
    fprintf( stdout, "\nLoading NEW camera parameters...\n\n" );
    if( OptotrakLoadCameraParameters( "NEW" ) )
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
    if( OptotrakActivateMarkers() )
    {
        goto ERROR_EXIT;
    } /* if */
	sleep( 1 );

    /*
     * Get and display ten frames of 3D data.  The position of which will be
     * in the new frame of reference.
     */
    fprintf( stdout, "\n\n3D Data Display\n" );
    for( uFrameCnt = 0; uFrameCnt < 10; ++uFrameCnt )
    {
        /*
         * Get a frame of data.
         */
        fprintf( stdout, "\n" );
        if( DataGetLatest3D( &uFrameNumber, &uElements, &uFlags, grdt3dData ) )
        {
            goto ERROR_EXIT;
        } /* if */

        /*
         * Print out the data.
         */
        fprintf( stdout, "Frame Number: %8u\n", uFrameNumber );
        fprintf( stdout, "Elements    : %8u\n", uElements );
        fprintf( stdout, "Flags       : 0x%04x\n", uFlags );
        for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; uMarkerCnt++ )
        {
			DisplayMarker( uMarkerCnt + 1, grdt3dData[uMarkerCnt] );
        } /* for */
    } /* for */
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
