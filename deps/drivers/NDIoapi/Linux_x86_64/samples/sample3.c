/*****************************************************************
Name:             SAMPLE3.C

Description:

    Optotrak Sample Program #3.

	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 10 frames of real-time Full Raw data.
	- Initialize data file for spooling 3D data.
	- Collect and spool 2 seconds of 3D data to file.
	- De-activate the markers.
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

#define NUM_SENSORS 3
#define NUM_MARKERS 6

/*****************************************************************
Static Structures and Types:
*****************************************************************/

/*
 * Typedef for sensor data returned in a full raw data packet.
 */
typedef struct SensorDataStruct
{
    unsigned char   ucPeak;
    unsigned char   ucDRC;
    unsigned char   ucCode;
    unsigned char   ucPeakNibble;
} SensorDataType;


/*
 * Typedef for a full raw data packet.
 */
typedef struct FullRawDataStruct
{
    float           fCentroid[ NUM_SENSORS];
    SensorDataType  SensorData[ NUM_SENSORS];
} FullRawDataType;

/*****************************************************************
Global Variables:
*****************************************************************/

/*
 * Array of strings for the sensor status codes.
 */
static char
    *pSensorStatusString[ 12] =
    {
        "CENTROID_OK",
        "CENTROID_WAVEFORM_TOO_WIDE",
        "CENTROID_PEAK_TOO_SMALL",
        "CENTROID_PEAK_TOO_LARGE",
        "CENTROID_WAVEFORM_OFF_DEVICE",
        "CENTROID_FELL_BEHIND",
        "CENTROID_LAST_CENTROID_BAD",
        "CENTROID_BUFFER_OVERFLOW",
        "CENTROID_MISSED_CCD"
		"CENTROID_ERROR_9",
		"CENTROID_ERROR_10",
		"CENTROID_ERROR_11",
		"CENTROID_ERROR_12"
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
    int
        nSensorCode;
    unsigned int
        uFlags,
        uElements,
        uFrameCnt,
        uMarkerCnt,
        uSensorCnt,
        uFrameNumber,
        uSpoolStatus = 0;
    static FullRawDataType
        pFullRawData[ NUM_MARKERS];
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #3\n\n" );

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
            (float)2.0,     /* Number of seconds of data to collect. */
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
     * Activate the markers.
     */
	fprintf( stdout, "...OptotrakActivateMarkers\n" );
    if( OptotrakActivateMarkers() )
    {
        goto ERROR_EXIT;
    } /* if */
	sleep( 1 );

    /*
     * Get and display ten frames of full raw data.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    for( uFrameCnt = 0; uFrameCnt < 10; ++uFrameCnt )
    {
        /*
         * Get a frame of data.
         */
        if( DataGetLatestRaw( &uFrameNumber, &uElements, &uFlags,
                              pFullRawData ) )
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

            /*
             * Print out the current marker number.
             */
            fprintf( stdout, "Marker %u\t\tCentroid Peak  DRC Code\n",
                uMarkerCnt + 1 );

            /*
             * Print out the information for each sensor.
             */
            for( uSensorCnt = 0; uSensorCnt < NUM_SENSORS; ++uSensorCnt )
            {

                /*
                 * Print out the current sensor number.
                 */
                fprintf( stdout, "\tSensor %u\t", uSensorCnt + 1 );

                /*
                 * Print out the centroid.  If is is bad print out the
                 * string 'missing'.
                 */
                if( pFullRawData[ uMarkerCnt].fCentroid[ uSensorCnt] <
                    MAX_NEGATIVE )
                {
                    fprintf( stdout, " missing " );
                } /* if */
                else
                {
                    fprintf( stdout, "%8.2f ",
                        pFullRawData[ uMarkerCnt].fCentroid[ uSensorCnt] );
                } /* else */

                /*
                 * Print out the rest of this sensor's information.
                 */
                nSensorCode =
                    pFullRawData[ uMarkerCnt].SensorData[ uSensorCnt].ucCode;
                fprintf( stdout, "%4d %4d %s\n",
                    pFullRawData[ uMarkerCnt].SensorData[ uSensorCnt].ucPeak,
                    pFullRawData[ uMarkerCnt].SensorData[ uSensorCnt].ucDRC,
                    pSensorStatusString[ nSensorCode] );
            } /* for */
        } /* for */
    } /* for */
    fprintf( stdout, "\n" );

    /*
     * Initialize a data file for spooling of the Optotrak data.
     */
    fprintf( stdout, "...DataBufferInitializeFile\n" );
    if( DataBufferInitializeFile( OPTOTRAK, "C#001.S03" ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Spool data to the previously initialized file.
     */
    fprintf( stdout, "...DataBufferSpoolData\n" );
    if( DataBufferSpoolData( &uSpoolStatus ) )
    {
        goto ERROR_EXIT;
    } /* if */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
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
