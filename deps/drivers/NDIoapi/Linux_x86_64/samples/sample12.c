/*****************************************************************
Name:             SAMPLE12.C

Description:

    Optotrak Sample Program #12.

	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an ODAU collection.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 10 frames of real-time ODAU Raw data.
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
        uFlags,
        uElements,
        uFrameCnt,
        uChannelCnt,
        uFrameNumber;
    static int
        puRawData[ NUM_CHANNELS + 1];
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #12\n\n" );

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
            0,                      /* Stream mode for the data buffers. */
            (float)1.0,             /* Number of seconds of data to collect. */
            0.0,                    /* Number of seconds to pre-trigger data by. */
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
            (float)100.0,   /* Frequency to collect data frames at. */
            (float)2500.0,  /* Marker frequency for marker maximum on-time. */
            30,             /* Dynamic or Static Threshold value to use. */
            160,            /* Minimum gain code amplification to use. */
            0,              /* Stream mode for the data buffers. */
            (float)0.4,    /* Marker Duty Cycle to use. */
            (float)7.5,     /* Voltage to use when turning on markers. */
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
     * Get and display ten frames of ODAU data.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    fprintf( stdout, "ODAU Data Display\n" );
    for( uFrameCnt = 0; uFrameCnt < 10; ++uFrameCnt )
    {

        /*
         * Get a frame of ODAU raw data.
         */
        fprintf( stdout, "\n" );
        if( DataGetLatestOdauRaw( ODAU1, &uFrameNumber, &uElements, &uFlags,
            puRawData ) )
        {
            goto ERROR_EXIT;
        } /* if */

        /*
         * Print out the data.
         */
        fprintf( stdout, "Frame Number: %8u\n", uFrameNumber );
        fprintf( stdout, "Elements    : %8u\n", uElements );
        fprintf( stdout, "Flags       : 0x%04x\n", uFlags );
        for( uChannelCnt = 0; uChannelCnt < NUM_CHANNELS; ++uChannelCnt )
        {
            fprintf( stdout, "Channel %u  Raw 0x%08x  Voltage %f\n",
                     uChannelCnt + 1,
                     puRawData[ uChannelCnt],
                     (float)( (int)( puRawData[ uChannelCnt])) *
                                     0.000305175/(float)ODAU_GAIN );
            /*
             * if you have an ODAUI unit, instead of an ODAUII unit,
             * replace the above fprintf statement with:
             * fprintf( stdout, "Channel %u  Raw 0x%08x  Voltage %f\n",
             *          uChannelCnt + 1,
             *          puRawData[ uChannelCnt ],
             *          (float)( (int)( puRawData[ uChannelCnt] & 0x0FFF ) - 2048) *
             *                          (10.0/2048.0)/(float)ODAU_GAIN );
             */
        } /* for */
        fprintf( stdout, "Digital 0x%04x\n", puRawData[ NUM_CHANNELS] );
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
