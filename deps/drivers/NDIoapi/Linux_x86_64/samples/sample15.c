/*****************************************************************
Name:             SAMPLE15.c

Description:

    Optotrak Sample Program #15.

	- Initiate communications with the Optotrak System.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive real-time 3D data until Marker_01 appears.
	- Initialize data file for spooling 3D data.
	- Collect and spool 5 seconds of 3D data to file.
	- De-activate the markers.
	- Disconnect from the Optotrak System.
	- Open the 3D data file for reading.
	- Open a new 3D data file for writing.
	- Normalize the contents of the collected data file and write
	  the results to the new data file.
	- Close the input and output data files.

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
#define COLLECTION_TIME (float)5.0

/*
 * Constants for identifying the input and output files.
 */
#define INPUT_FILE      0
#define OUTPUT_FILE     1

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
        nItemCnt,
        nFileItems,
        nFileSubItems;
    void
        *pFileHeader = NULL;
    float
        fFileFrequency;
    long int
        lnFrameCnt,
        lnFileFrames;
    unsigned int
        uFlags,
        uElements,
        uFrameNumber,
        uSpoolStatus = 0;
    static char
        szFileComments[ 81];
    static Position3d
        pBase3dData[ NUM_MARKERS],
        pInput3dData[ NUM_MARKERS],
        pOutput3dData[ NUM_MARKERS];
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #15\n\n" );

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
     * Loop until marker 1 comes into view.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    fprintf( stdout, "Waiting for marker 1...\n" );
    do
    {

        /*
         * Get a frame of 3D data.
         */
        if( DataGetLatest3D( &uFrameNumber, &uElements, &uFlags, pBase3dData ) )
        {
            goto ERROR_EXIT;
        } /* if */
    } /* do */
    while( pBase3dData[ 0].x < MAX_NEGATIVE );

    /*
     * Initialize a file for spooling of the Optotrak 3D data.
     */
    if( DataBufferInitializeFile( OPTOTRAK, "C#001.S15" ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Spool data to the previously initialized file.
     */
    fprintf( stdout, "Collecting data file...\n" );
    if( DataBufferSpoolData( &uSpoolStatus ) )
    {
        goto ERROR_EXIT;
    } /* if */
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
     * Open the 3D data file we just collected as our input data file.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    fprintf( stdout, "Processing 3D data file...\n" );
    if( OAPIFileOpen( "C#001.S15",
                  INPUT_FILE,
                  OPEN_READ,
                  &nFileItems,
                  &nFileSubItems,
                  &lnFileFrames,
                  &fFileFrequency,
                  szFileComments,
                  &pFileHeader ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Open a new file as our output file.
     */
    strcpy( szFileComments, "Normalized 3D data file" );
    if( OAPIFileOpen( "NC#001.S15",
                  OUTPUT_FILE,
                  OPEN_WRITE,
                  &nFileItems,
                  &nFileSubItems,
                  &lnFileFrames,
                  &fFileFrequency,
                  szFileComments,
                  &pFileHeader ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Read the first frame of 3D data from the input file and store it for
     * future use.
     */
    if( OAPIFileRead( INPUT_FILE, 0L, 1, pBase3dData ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Read each frame of the input file and subtract the base position.
     * Write the resultant frame to the output file.
     */
    for( lnFrameCnt = 0L; lnFrameCnt < lnFileFrames; ++lnFrameCnt )
    {

        /*
         * Read the current frame from the input file.
         */
        if( OAPIFileRead( INPUT_FILE, lnFrameCnt, 1, pInput3dData ) )
        {
            goto ERROR_EXIT;
        } /* if */

        /*
         * Perform the subtraction for each in item in the frame.
         */
        for( nItemCnt = 0; nItemCnt < nFileItems; ++nItemCnt )
        {

            /*
             * If the input data for this marker is missing set the output
             * data to missing as well.
             */
            if( pInput3dData[ nItemCnt].x < MAX_NEGATIVE )
            {
                pOutput3dData[ nItemCnt].x =
                pOutput3dData[ nItemCnt].y =
                pOutput3dData[ nItemCnt].z = BAD_FLOAT;
           } /* if */

            /*
             * Else perform the subtraction.
             */
            else
            {
                pOutput3dData[ nItemCnt].x = pInput3dData[ nItemCnt].x -
                    pBase3dData[ nItemCnt].x;
                pOutput3dData[ nItemCnt].y = pInput3dData[ nItemCnt].y -
                    pBase3dData[ nItemCnt].y;
                pOutput3dData[ nItemCnt].z = pInput3dData[ nItemCnt].z -
                    pBase3dData[ nItemCnt].z;
            } /* else */
        } /* for */

        /*
         * Write the calculated frame to the output file.
         */
        if( OAPIFileWrite( OUTPUT_FILE, lnFrameCnt, 1, pOutput3dData ) )
        {
            goto ERROR_EXIT;
        } /* if */
    } /* for */

    /*
     * Close the input and output files.
     */
    OAPIFileClose( INPUT_FILE );
    OAPIFileClose( OUTPUT_FILE );
    fprintf( stdout, "File processing complete.\n" );
    fprintf( stdout, "\n" );

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
