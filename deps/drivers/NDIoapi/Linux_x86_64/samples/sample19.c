/*****************************************************************
Name:             SAMPLE19.C

Description:

    Optotrak Sample Program #19.

	- Initiate communications with the Optotrak System.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Add rigid body to the tracking list using an array of 3D points.
	- Activate the markers.
	- Initialize a data file for spooling Raw Optotrak data.
	- Collect and spool 5 seconds of Raw data to file.
	- De-activate the markers.
	- Open the Raw data file for reading.
	- Read the entire contents of the raw data file.
	- Close the data file.
	- Convert the raw data to 3D data and display the data.
	- Convert the 3D data to transformation data and display the data.
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

#define NUM_MARKERS     6
#define NUM_SENSORS     3
#define FRAME_RATE      (float)20.0
#define COLLECTION_TIME (float)5.0

/*
 * Constants for identifying the files.
 */
#define INPUT_FILE      0

/*
 * Constants for identifying the rigid bodies.
 */
#define NUM_RIGID_BODIES    1
#define RIGID_BODY_ID       0

/*****************************************************************
Global Variables:
*****************************************************************/
/*
 * Modify these values with the attached rigid body's definition
 */
static Position3d
    dtRigidBody[ NUM_MARKERS] =
    {
        { 2.1985F, -15.8936F,   86.8858F },
        { 2.1985F,  15.7071F,   86.8858F },
        { 1.7975F,  15.7819F,  131.3168F },
        { 1.5955F,  15.7645F,  175.8451F },
        { 1.8280F, -15.7077F,  175.8297F },
        { 2.1985F, -15.8136F,  131.3926F }
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
        nFileItems,
        nFileSubItems;
    void
        *pFileHeader;
    float
        fFileFrequency,
        *pfRawDataCur = NULL;
    long int
        lnFrameCnt,
        lnFileFrames;
    unsigned int
        uElements,
        uElementCnt,
        uSpoolStatus = 0;
    static char
        szFileComments[ 81];
    float
        *pfRawData = NULL;
    Position3d
        *pdtPositionData = NULL,
        *pdtPositionCur  = NULL;
    struct OptotrakRigidStruct
        dtRigidBodyData;
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #19\n\n" );

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
     * Set optional processing flags (this overides the settings in Optotrak.INI).
     */
	fprintf( stdout, "...OptotrakSetProcessingFlags\n" );
    if( OptotrakSetProcessingFlags( OPTO_LIB_POLL_REAL_DATA |
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
     * Set up a collection for the Optotrak.
     */
	fprintf( stdout, "...OptotrakSetupCollection\n" );
    if( OptotrakSetupCollection(
            NUM_MARKERS,        /* Number of markers in the collection. */
            FRAME_RATE,         /* Frequency to collect data frames at. */
            (float)2500.0,      /* Marker frequency for marker maximum on-time. */
            30,                 /* Dynamic or Static Threshold value to use. */
            160,                /* Minimum gain code amplification to use. */
            0,                  /* Stream mode for the data buffers. */
            (float)0.4,         /* Marker Duty Cycle to use. */
            (float)7.5,         /* Voltage to use when turning on markers. */
            COLLECTION_TIME,    /* Number of seconds of data to collect. */
            (float)0.0,         /* Number of seconds to pre-trigger data by. */
            OPTOTRAK_NO_FIRE_MARKERS_FLAG | OPTOTRAK_BUFFER_RAW_FLAG | OPTOTRAK_GET_NEXT_FRAME_FLAG ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Add the rigid body to the Optotrak system from an array of 3D points.
     */
	fprintf( stdout, "...RigidBodyAdd\n" );
    if( RigidBodyAdd(
            RIGID_BODY_ID,          /* ID associated with this rigid body. */
            1,                      /* First marker in the rigid body. */
            NUM_MARKERS,            /* Number of markers in the rigid body. */
            (float *)dtRigidBody,   /* 3D coords for each marker in the body. */
            NULL,                   /* no normals for this rigid body. */
            OPTOTRAK_QUATERN_RIGID_FLAG | OPTOTRAK_RETURN_EULER_FLAG ) )
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
     * Initialize a file for spooling of the Optotrak raw data.
     */
	fprintf( stdout, "...DataBufferInitializeFile\n" );
    if( DataBufferInitializeFile( OPTOTRAK, "R#001.S19" ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Spool data to the previously initialized file.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
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
     * Open the raw data file we just collected as our input data file.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    fprintf( stdout, "Processing raw data file...\n" );
    if( OAPIFileOpen( "R#001.S19",
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
     * Allocate the memory required for the conversions and transformations.
     */
    pfRawData = (float *)malloc( lnFileFrames * nFileItems * nFileSubItems *
                                 sizeof(float) );
    if( pfRawData == NULL )
    {
        fprintf( stderr, "Insufficient memory for raw data." );
        goto ERROR_EXIT;
    } /* if */
    pdtPositionData = (Position3d *)malloc( lnFileFrames * nFileItems *
                                            nFileSubItems * sizeof(Position3d) );
    if( pdtPositionData == NULL )
    {
        fprintf( stderr, "Insufficient memory for 3D position data." );
        goto ERROR_EXIT;
    } /* if */

    /*
     * Read the raw data from the file.
     */
    if( OAPIFileRead( INPUT_FILE, 0L, (int)lnFileFrames, pfRawData ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Close the input file.
     */
    OAPIFileClose( INPUT_FILE );
    fprintf( stdout, "File processing complete.\n" );

    /*
     * Convert the raw data to 3D position data and display it.
     */
    fprintf( stdout, "\n3D position data:" );
    pfRawDataCur   = pfRawData;
    pdtPositionCur = pdtPositionData;
    for( lnFrameCnt = 0; lnFrameCnt < lnFileFrames; lnFrameCnt++ )
    {
        if( OptotrakConvertRawTo3D( &uElements,
                                    pfRawDataCur,
                                    pdtPositionCur ) )
        {
            goto ERROR_EXIT;
        } /* if */

        for( uElementCnt = 0; uElementCnt < uElements; uElementCnt++ )
        {
            fprintf( stdout,
                    "\n%5ld %3u %12.5f %12.5f %12.5f",
                    lnFrameCnt,
                    uElementCnt,
                    pdtPositionCur[uElementCnt].x,
                    pdtPositionCur[uElementCnt].y,
                    pdtPositionCur[uElementCnt].z );
        } /* for */
        pfRawDataCur += uElements * NUM_SENSORS;
        pdtPositionCur += uElements;
    } /* for */

    /*
     * Transform the 3D position data to 6D and display it.
     */
    fprintf( stdout, "\n\nRigid body data:\n" );
    pdtPositionCur = pdtPositionData;
    for( lnFrameCnt = 0; lnFrameCnt < lnFileFrames; lnFrameCnt++ )
    {
        if( OptotrakConvertTransforms( &uElements,
                                       &dtRigidBodyData,
                                       pdtPositionCur ) )
        {
            goto ERROR_EXIT;
        } /* if */

        fprintf( stdout, "Frame %04u\n", lnFrameCnt );
        fprintf( stdout,
                 "XT = %12.6f YT = %12.6f ZT = %12.6f\n",
                 dtRigidBodyData.transformation.euler.translation.x,
                 dtRigidBodyData.transformation.euler.translation.y,
                 dtRigidBodyData.transformation.euler.translation.z );
        fprintf( stdout,
                 "Y  = %12.6f P  = %12.6f R  = %12.6f\n",
                 dtRigidBodyData.transformation.euler.rotation.yaw,
                 dtRigidBodyData.transformation.euler.rotation.pitch,
                 dtRigidBodyData.transformation.euler.rotation.roll );
        pdtPositionCur += uElementCnt;
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
    free( pfRawData );
    free( pdtPositionData );
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

    if( pfRawData != NULL )
	{
        free( pfRawData );
	} /* if */

    if( pdtPositionData != NULL )
	{
        free( pdtPositionData );
	} /* if */

    exit( 1 );
} /* main */
