/*****************************************************************
Name:             SAMPLE10.C

Description:

    Optotrak Sample Program #10.

	- Initiate communications with the Optotrak System.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Add rigid body to the tracking list using a rigid body file.
	- Change the settings for the rigid body.
	- Request/receive/display 10 frames of real-time rigid body
	  transformations and associated 3D data.
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

#define NUM_MARKERS     6
#define FRAME_RATE      (float)50.0
#define COLLECTION_TIME (float)5.0

/*
 * Constants for identifying the rigid bodies.
 */
#define RIGID_BODY_1        0
#define NUM_RIGID_BODIES    1

/*****************************************************************
Static Structures and Types:
*****************************************************************/


/*
 * Type definition to retreive and access rigid body transformation
 * data.
 */
typedef struct RigidBodyDataStruct
{
    struct OptotrakRigidStruct  pRigidData[ NUM_RIGID_BODIES];
    Position3d                  p3dData[ NUM_MARKERS];
} RigidBodyDataType;

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
        uRigidCnt,
        uMarkerCnt,
        uFrameNumber;
    RigidBodyDataType
        RigidBodyData;
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak sample program #10\n\n" );

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
     * Add rigid body 1 for tracking to the Optotrak system from a .RIG file.
     */
	fprintf( stdout, "...RigidBodyAddFromFile\n" );
    if( RigidBodyAddFromFile(
            RIGID_BODY_1,   /* ID associated with this rigid body. */
            1,              /* First marker in the rigid body.*/
            "plate",        /* RIG file containing rigid body coordinates.*/
            0 ) )           /* flags */
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Change the default settings for this rigid body 1.
     */
	fprintf( stdout, "...RigidBodyChangeSettings\n" );
    if( RigidBodyChangeSettings(
            RIGID_BODY_1,   /* ID associated with this rigid body. */
            4,              /* Minimum number of markers which must be seen
                               before performing rigid body calculations.*/
            60,             /* Cut off angle for marker inclusion in calcs.*/
            (float)0.25,    /* Maximum 3-D marker error for this rigid body. */
            (float)1.0,     /* Maximum raw sensor error for this rigid body. */
            (float)1.0,     /* Maximum 3-D RMS marker error for this rigid body. */
            (float)1.0,     /* Maximum raw sensor RMS error for this rigid body. */
            OPTOTRAK_QUATERN_RIGID_FLAG | OPTOTRAK_RETURN_QUATERN_FLAG ) )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * Get and display ten frames of rigid body data.
     */
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    fprintf( stdout, "Rigid Body Data Display\n" );
    for( uFrameCnt = 0; uFrameCnt < 10; ++uFrameCnt )
    {

        /*
         * Get a frame of data.
         */
        if( DataGetLatestTransforms( &uFrameNumber, &uElements, &uFlags,
                                     &RigidBodyData ) )
        {
            goto ERROR_EXIT;
        } /* if */

        /*
         * Print out the rigid body transformation data.
         */
        fprintf( stdout, "\n" );
        fprintf( stdout, "Rigid Body Transformation Data\n\n" );
        fprintf( stdout, "Frame Number: %8u\n", uFrameNumber );
        fprintf( stdout, "Transforms  : %8u\n", uElements );
        fprintf( stdout, "Flags       :   0x%04x\n", uFlags );
        for( uRigidCnt = 0; uRigidCnt < uElements; ++uRigidCnt )
        {
            fprintf( stdout, "Rigid Body %u\n",
                     RigidBodyData.pRigidData[ uRigidCnt].RigidId );
            fprintf( stdout, "XT = %8.2f YT = %8.2f ZT = %8.2f\n",
                     RigidBodyData.pRigidData[ uRigidCnt].transformation.
                         quaternion.translation.x,
                     RigidBodyData.pRigidData[ uRigidCnt].transformation.
                         quaternion.translation.y,
                     RigidBodyData.pRigidData[ uRigidCnt].transformation.
                         quaternion.translation.z );
            fprintf( stdout, "Q0 = %8.2f QX = %8.2f QY = %8.2f QZ = %8.2f\n",
                     RigidBodyData.pRigidData[ uRigidCnt].transformation.
                         quaternion.rotation.q0,
                     RigidBodyData.pRigidData[ uRigidCnt].transformation.
                         quaternion.rotation.qx,
                     RigidBodyData.pRigidData[ uRigidCnt].transformation.
                         quaternion.rotation.qy,
                     RigidBodyData.pRigidData[ uRigidCnt].transformation.
                         quaternion.rotation.qz );
        } /* for */

        /*
         * Print out the 3D data.
         */
        fprintf( stdout, "\nAssociated 3D Marker Data\n\n" );
        for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; ++uMarkerCnt )
        {
			DisplayMarker( uMarkerCnt + 1, RigidBodyData.p3dData[uMarkerCnt] );
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
