/*****************************************************************
Name:             SAMPLE16.C

Description:

    Optotrak Sample Program #16.

	- Initiate communications with the Optotrak System
	  as a secondary host.
	- Request the current Optotrak System status.
	- Request/receive/display 10 frames of Full Raw data.
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
#define NUM_MARKERS 6
#define NUM_SENSORS 3

/*****************************************************************
Static Routines:
*****************************************************************/

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
    *pSensorStatusString[ 13] =
    {
        "CENTROID_OK",
        "CENTROID_WAVEFORM_TOO_WIDE",
        "CENTROID_PEAK_TOO_SMALL",
        "CENTROID_PEAK_TOO_LARGE",
        "CENTROID_WAVEFORM_OFF_DEVICE",
        "CENTROID_FELL_BEHIND",
        "CENTROID_LAST_CENTROID_BAD",
        "CENTROID_BUFFER_OVERFLOW",
        "CENTROID_MISSED_CCD",
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
        nFlags,
        nNumSensors,
        nNumOdaus,
        nRigidBodies,
        nMarkers,
        nMarkerCnt,
        nThreshold,
        nMinimumGain,
        nStreamData,
        nSensorCode;
    float
        fVoltage,
        fDutyCycle,
        fCollectionTime,
        fPreTriggerTime,
        fFrameFrequency,
        fMarkerFrequency;
    unsigned int
        uFlags,
        uElements,
        uFrameCnt,
        uSensorCnt,
        uFrameNumber;
    FullRawDataType
        *pFullRawData;
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];
	boolean
		bNotSecondaryHost = FALSE;

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #16\n\n" );

	/*
	 * look for the -nosecondary parameter that indicates 'not used as secondary host
	 */
	if( ( argc >= 2 ) && ( strncmp( argv[1], "-nosecondary", 12 ) == 0 ) )
	{
		bNotSecondaryHost = TRUE;
	} /* if */

	if( bNotSecondaryHost )
	{
		/*
		 * look for the -nodld parameter that indicates 'no download'
		 */
		if( ( argc < 3 ) || ( strncmp( argv[2], "-nodld", 6 ) != 0 ) )
		{
			/*
			 * Load the system of processors.
			 */
			fprintf( stdout, "...TransputerLoadSystem\n" );
			if( TransputerLoadSystem( "system" ) != OPTO_NO_ERROR_CODE )
			{
				goto ERROR_EXIT;
			} /* if */
		} /* if */

		/*
		 * Wait one second to let the system finish loading.
		 */
		sleep( 1 );

	} /* if */


    /*
     * Initialize the processors system.
     */
	fprintf( stdout, "...TransputerInitializeSystem\n" );
    if( TransputerInitializeSystem( OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG |
                                    OPTO_SECONDARY_HOST_FLAG ) )
    {
        goto ERROR_EXIT;
    } /* if */

	if( bNotSecondaryHost )
	{
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

		/*
		 * Set the strober port table
		 */
		fprintf( stdout, "...OptotrakSetStroberPortTable\n" );
		if( OptotrakSetStroberPortTable( NUM_MARKERS, 0, 0, 0 ) )
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

	} /* if */

    /*
     * Request and receive the Optotrak status.
     */
	fprintf( stdout, "...OptotrakGetStatus\n" );
    if( OptotrakGetStatus(
            &nNumSensors,
            &nNumOdaus,
            &nRigidBodies,
            &nMarkers,
            &fFrameFrequency,
            &fMarkerFrequency,
            &nThreshold,
            &nMinimumGain,
            &nStreamData,
            &fDutyCycle,
            &fVoltage,
            &fCollectionTime,
            &fPreTriggerTime,
            &nFlags ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Allocate memory for receiving the real-time Optotrak raw data.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    pFullRawData = (FullRawDataType *)calloc( nMarkers,
                                              sizeof( FullRawDataType ) );
    if( NULL == pFullRawData )
    {
        fprintf( stdout, "Error: Unable to allocate required memory.\n" );
        TransputerShutdownSystem();
        exit( 1 );
    } /* if */

    /*
     * Get and display ten frames of Optotrak Raw data.
     */
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
        for( nMarkerCnt = 0; nMarkerCnt < nMarkers; ++nMarkerCnt )
        {

            /*
             * Print out the current marker number.
             */
            fprintf( stdout, "Marker %u\t\tCentroid Peak  DRC Code\n",
                nMarkerCnt + 1 );

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
                if( pFullRawData[ nMarkerCnt].fCentroid[ uSensorCnt] <
                    MAX_NEGATIVE )
                {
                    fprintf( stdout, " missing " );
                } /* if */
                else
                {
                    fprintf( stdout, "%8.2f ",
                             pFullRawData[ nMarkerCnt].fCentroid[ uSensorCnt] );
                } /* else */

                /*
                 * Print out the rest of this sensor's information.
                 */
                nSensorCode =
                    pFullRawData[ nMarkerCnt].SensorData[ uSensorCnt].ucCode;
                fprintf( stdout, "%4d %4d %s\n",
                    pFullRawData[ nMarkerCnt].SensorData[ uSensorCnt].ucPeak,
                    pFullRawData[ nMarkerCnt].SensorData[ uSensorCnt].ucDRC,
                    pSensorStatusString[ nSensorCode] );
            } /* for */
        } /* for */
    } /* for */
    fprintf( stdout, "\n" );

	if( bNotSecondaryHost )
	{
		/*
		 * deactivate the optotrak markers
		 */
		OptotrakDeActivateMarkers();
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
