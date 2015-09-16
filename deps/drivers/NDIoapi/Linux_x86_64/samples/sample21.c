/*****************************************************************
Name:             SAMPLE21.C

Description:

    Optotrak Sample Program #21.

	- Set processing flags to perform data conversions on the host computer.
	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Obtain/display the camera parameter status.
	- Select a new camera parameter set.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 10 frames of real-time 3D data.
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

#define NUM_SENSORS     3
#define NUM_MARKERS     6
#define MARKER_TYPE     1
#define WAVELENGTH_TYPE 0
#define MODEL_TYPE      0

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
        uFrameNumber;
    int
        nMarkerType,
        nWavelengthType,
        nModelType;
    static Position3d
        p3dData[ NUM_MARKERS];
    char
        szCamParmSetInfo[ 1000 ],
        szNDErrorString[ MAX_ERROR_STRING_LENGTH + 1];

    /*
     * Set optional processing flags.
     */
    if( OptotrakSetProcessingFlags( OPTO_LIB_POLL_REAL_DATA |
                                    OPTO_CONVERT_ON_HOST |
                                    OPTO_RIGID_ON_HOST ) )
    {
        goto ERROR_EXIT;
    } /* if */

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #21\n\n" );

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
     * Obtain information concerning the available camera parameter
     * sets stored in this file, and get the marker type, wavelength
     * and model type for the parameter set currently used in the API.
     */
    if( OptotrakGetCameraParameterStatus( &nMarkerType,
                                          &nWavelengthType,
                                          &nModelType,
                                          szCamParmSetInfo,
                                          sizeof( szCamParmSetInfo ) ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Print out a the status information
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    fprintf( stdout, "Camera Parameter Information\n%s\n", szCamParmSetInfo);

    /*
     * Select a new parameter set specified by the given marker type,
     * wavelength and model type. This will fail if the camera parameters
     * contain only one set of parameters.
     */
    nMarkerType     = MARKER_TYPE;
    nWavelengthType = WAVELENGTH_TYPE;
    nModelType      = MODEL_TYPE;
    if( OptotrakSetCameraParameters( nMarkerType, nWavelengthType, nModelType ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Set up a collection for the Optotrak.
     */
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
    if( OptotrakActivateMarkers() )
    {
        goto ERROR_EXIT;
    } /* if */
	sleep( 1 );

    /*
     * Get and display ten frames of 3D data.
     */
    fprintf( stdout, "\n\n3D Data Display\n" );
    for( uFrameCnt = 0; uFrameCnt < 10; ++uFrameCnt )
    {
        /*
         * Get a frame of data.
         */
        fprintf( stdout, "\n" );
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
        for( uMarkerCnt = 0; uMarkerCnt < 6; ++uMarkerCnt )
        {
			DisplayMarker( uMarkerCnt + 1, p3dData[uMarkerCnt] );
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
