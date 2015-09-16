/*****************************************************************
Name:             SAMPLE4.C

Description:

    Optotrak Sample Program #4.

	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Initialize a memory block for spooling Raw Optotrak data.
	- Collect and spool 2 seconds of Raw data to memory.
	- De-activate the markers.
	- Display the 100 frames of data stored in the memory block.
	- Disconnect from the Optotrak System.

NOTE: This sample is intended for use with one position sensor only.

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
#define FRAME_RATE      (float)50.0
#define COLLECTION_TIME (float)2.0

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
    fTemp,
        *pfRawDataBuffer = NULL;
    unsigned char
        *pchRawDataPtr = NULL;
    unsigned int
        uFrameCnt,
        uMarkerCnt,
        uSensorCnt,
        uMemoryRequired,
        uSpoolStatus = 0;
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #4\n\n" );

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
            FRAME_RATE,     /* Frequency to collect data frames at. */
            (float)2500.0,  /* Marker frequency for marker maximum on-time. */
            30,             /* Dynamic or Static Threshold value to use. */
            160,            /* Minimum gain code amplification to use. */
            0,              /* Stream mode for the data buffers. */
            (float)0.4,     /* Marker Duty Cycle to use. */
            (float)7.5,     /* Voltage to use when turning on markers. */
            COLLECTION_TIME,/* Number of seconds of data to collect. */
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
     * Allocate enough memory to store the Optotrak raw data.
     */
    uMemoryRequired = (unsigned int)( COLLECTION_TIME * FRAME_RATE *
                        NUM_MARKERS * NUM_SENSORS * sizeof( float ) );
    pfRawDataBuffer = (float *)malloc( uMemoryRequired );
    if( NULL == pfRawDataBuffer )
    {
        fprintf( stderr, "Error: Unable to allocate memory.\n" );
        goto ERROR_EXIT;
    } /* if */
    memset( pfRawDataBuffer, 0x00, uMemoryRequired );

    /*
     * Initialize an area of memory for spooling of the Optotrak data.
     */
	fprintf( stdout, "...DataBufferInitializeMem\n" );
    if( DataBufferInitializeMem( OPTOTRAK, pfRawDataBuffer ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Spool data to the previously initialized memory area.
     */
	fprintf( stdout, "...DataBufferSpoolData\n" );
    if( DataBufferSpoolData( &uSpoolStatus ) )
    {
        goto ERROR_EXIT;
    } /* if */
    fprintf( stdout, "Spool Status: 0x%04x\n", uSpoolStatus );

    /*
     * De-activate the markers.
     */
	fprintf( stdout, "...OptotrakDeActivateMarkers\n" );
    if( OptotrakDeActivateMarkers() )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Print out the data spool from memory.
     */
    pchRawDataPtr = (unsigned char *)pfRawDataBuffer;
    for( uFrameCnt = 0; uFrameCnt < COLLECTION_TIME * FRAME_RATE; ++uFrameCnt )
    {
        /*
         * Print out the current frame.
         */
        fprintf( stdout, "\n\nFrame %u\n", uFrameCnt + 1 );

        /*
         * Print the data for each marker in this frame.
         */
        for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; ++uMarkerCnt )
        {

             /*
              * Print out the current marker.
              */
            fprintf( stdout, "\nMarker %u", uMarkerCnt + 1 );

            /*
             * Print the data from each sensor.
             */
            for( uSensorCnt = 0; uSensorCnt < NUM_SENSORS; ++uSensorCnt )
            {
                pchRawDataPtr += UnPackFloat( &fTemp, pchRawDataPtr, PC_FLOAT_SIZE );
                fprintf( stdout, " Sensor%02d %7.2f", uSensorCnt + 1, fTemp );
            } /* for */
        } /* for */
    } /* for */
    fprintf( stdout, "\n\n" );

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
    if( NULL != pfRawDataBuffer )
    {
        free( pfRawDataBuffer );
    } /* if */
    fprintf( stdout, "\nProgram execution complete.\n" );
    exit( 0 );

ERROR_EXIT:
	/*
	 * Indicate that an error has occurred
	 */
	fprintf( stdout, "\nAn error has occurred during execution of the program.\n" );
    if( NULL != pfRawDataBuffer )
    {
        free( pfRawDataBuffer );
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
