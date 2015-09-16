/*****************************************************************
Name:             CERTUS_SAMPLE14.C

Description:
	Optotrak Certus Sample 14

	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set number of markers to activate on each strober.
	- Add rigid body to tracking list from device SROMs.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Retrieve/display 1 frame of 6D data.
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

#define SAMPLE_MARKERFREQ	2500.0f
#define SAMPLE_FRAMEFREQ	30.0f
#define SAMPLE_DUTYCYCLE	0.35f
#define SAMPLE_VOLTAGE		7.0f
#define SAMPLE_STREAMDATA	0

#define NUM_MARKERS			6
#define NUM_RIGID_BODIES    1
#define RIGID_BODY_ID       0

#define CERTUS_SAMPLE_STROBER_MARKERSTOFIRE		2
#define CERTUS_SAMPLE_3020_STROBER_MARKERSTOFIRE	6

/*****************************************************************
Static Structures and Types:
*****************************************************************/


/*****************************************************************
Application Files Included
*****************************************************************/
#include "certus_aux.h"
#include "ot_aux.h"


void main( int argc, unsigned char *argv[] )
{
	OptotrakSettings
		dtSettings;
	unsigned int
		uFrameNumber,
		uElements,
		uFlags;
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];
    int
		i,
		nMarkersToActivate,
		nCurDevice,
		nCurRigid,
		nDevices,
		nTotalRigids;
	ApplicationDeviceInformation
		*pdtDevices;
    DeviceHandle
        *pdtDeviceHandles;
    DeviceHandleInfo
        *pdtDeviceHandlesInfo;
    Position3d
        *pData3d;
	struct OptotrakRigidStruct
        *pData6d;


    /*
     * initialization
	 * intialize variables
     */
	pdtDevices = NULL;
	pdtDeviceHandles = NULL;
	pdtDeviceHandlesInfo = NULL;
	pData6d = NULL;
	pData3d = NULL;
	dtSettings.nMarkers = 0;
	dtSettings.fFrameFrequency = SAMPLE_FRAMEFREQ;
	dtSettings.fMarkerFrequency = SAMPLE_MARKERFREQ;
	dtSettings.nThreshold = 30;
	dtSettings.nMinimumGain = 160;
	dtSettings.nStreamData = SAMPLE_STREAMDATA;
	dtSettings.fDutyCycle = SAMPLE_DUTYCYCLE;
	dtSettings.fVoltage = SAMPLE_VOLTAGE;
	dtSettings.fCollectionTime = 1.0;
	dtSettings.fPreTriggerTime = 0.0;
	nDevices = 0;
	nTotalRigids = 0;
	nMarkersToActivate = 0;

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak Certus sample program #14\n\n" );

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
     * Communication Initialization
     * Once the system processors have been loaded, the application
     * prepares for communication by initializing the system processors.
     */
	fprintf( stdout, "...TransputerInitializeSystem\n" );
    if( TransputerInitializeSystem( OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	 * Determine if this sample will run with the system attached.
	 * This sample is intended for Certus systems.
	 */
	fprintf( stdout, "...DetermineSystem\n" );
	if( uDetermineSystem( ) != OPTOTRAK_CERTUS_FLAG )
	{
		goto PROGRAM_COMPLETE;
	} /* if */

    /*
     * Strober Initialization
     * Once communication has been initialized, the application must
     * determine the strober configuration.
     * The application retrieves device handles and all strober
     * properties from the system.
     */
	fprintf( stdout, "...DetermineStroberConfiguration\n" );
	if( DetermineStroberConfiguration( &pdtDeviceHandles, &pdtDeviceHandlesInfo, &nDevices ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	 * check if any devices have been detected by the system
	 */
	if( nDevices == 0 )
	{
		fprintf( stdout, ".........no devices detected.  Quitting program...\n" );
		goto PROGRAM_COMPLETE;
	} /* if */

    /*
     * Now that all the device handles have been completely set up,
     * the application can store all the device handle information in
     * an internal data structure.  This will facilitate lookups when
     * a property setting needs to be checked.
     */
    ApplicationStoreDeviceProperties( &pdtDevices, pdtDeviceHandlesInfo, nDevices );

	/*
	 * Add rigid body data from device handle
	 */
	for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
	{
		/*
		 * if the device has a ROM, use the rigid body from the rom to set up the collection
		 */
		if( pdtDevices[nCurDevice].bHasROM )
		{
			fprintf( stdout, "...RigidBodyAddFromDeviceHandle\n" );
			if( RigidBodyAddFromDeviceHandle( pdtDeviceHandlesInfo[nCurDevice].pdtHandle->nID, nTotalRigids, 0 ) != OPTO_NO_ERROR_CODE )
			{
				goto ERROR_EXIT;
			} /* if */

			nTotalRigids++;
		}
		else
		{
			nMarkersToActivate = pdtDevices[nCurDevice].b3020Capability?
								   CERTUS_SAMPLE_3020_STROBER_MARKERSTOFIRE : CERTUS_SAMPLE_STROBER_MARKERSTOFIRE;

			SetMarkersToActivateForDevice( &(pdtDevices[nCurDevice]), pdtDeviceHandlesInfo[nCurDevice].pdtHandle->nID, nMarkersToActivate );
		} /* for */
	} /* for */

	/*
	 * Determine the collection settings based on the device properties
	 */
	ApplicationDetermineCollectionParameters( nDevices, pdtDevices, &dtSettings );


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
     * Continue system initialization by
     * loading the appropriate camera parameters.
     */
	fprintf( stdout, "...OptotrakLoadCameraParameters\n" );
    if( OptotrakLoadCameraParameters( "standard" ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	 * Ensure that we are firing some markers
	 */
	if( dtSettings.nMarkers == 0 )
	{
		fprintf( stdout, "Error: There are no markers to be activated.\n" );
		goto ERROR_EXIT;
	} /* if */

    /*
     * Configure Optotrak Collection
     * Once the system strobers have been enabled, and all settings are
     * loaded, the application can set up the Optotrak collection
     */
	fprintf( stdout, "...OptotrakSetupCollection\n" );
	fprintf( stdout, ".....%d, %.2f, %.0f, %d, %d, %d, %.2f, %.2f, %.0f, %.0f\n",
								 dtSettings.nMarkers,
			                     dtSettings.fFrameFrequency,
				                 dtSettings.fMarkerFrequency,
					             dtSettings.nThreshold,
						         dtSettings.nMinimumGain,
							     dtSettings.nStreamData,
								 dtSettings.fDutyCycle,
								 dtSettings.fVoltage,
								 dtSettings.fCollectionTime,
								 dtSettings.fPreTriggerTime );
    if( OptotrakSetupCollection( dtSettings.nMarkers,
			                     dtSettings.fFrameFrequency,
				                 dtSettings.fMarkerFrequency,
					             dtSettings.nThreshold,
						         dtSettings.nMinimumGain,
							     dtSettings.nStreamData,
								 dtSettings.fDutyCycle,
								 dtSettings.fVoltage,
								 dtSettings.fCollectionTime,
								 dtSettings.fPreTriggerTime,
								 OPTOTRAK_NO_FIRE_MARKERS_FLAG | OPTOTRAK_BUFFER_RAW_FLAG | OPTOTRAK_SWITCH_AND_CONFIG_FLAG ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */
	sleep( 1 );

	/*
	 * allocate memory for data
	 */
	pData3d = (Position3d*)malloc( dtSettings.nMarkers * sizeof( Position3d ) );
	pData6d = (struct OptotrakRigidStruct*)malloc( nTotalRigids * sizeof( struct OptotrakRigidStruct ) );

    /*
     * Prepare for realtime data retrieval.
     * Activate markers. Turn on the markers prior to data retrieval.
     */
    OptotrakActivateMarkers( );
	sleep( 1 );

    /*
     * Retrieve a frame of 6D data.
     */
	/*
     * Get a frame of data.
     */
    if( DataGetLatestTransforms2( &uFrameNumber, &uElements, &uFlags, pData6d, pData3d ) )
    {
        goto ERROR_EXIT;
    } /* if */

	/*
	 * display the 3d data
	 */
    fprintf( stdout, "\n\nSample Program Results:\n" );
    /*
     * Check the returned flags member for improper transforms.
     */
	for( nCurRigid = 0; nCurRigid < nTotalRigids; ++nCurRigid )
	{
		if( pData6d[nCurRigid].flags & OPTOTRAK_UNDETERMINED_FLAG )
		{
			fprintf( stdout, "Undetermined transform!\n" );
			if( pData6d[nCurRigid].flags & OPTOTRAK_RIGID_ERR_MKR_SPREAD )
			{
				fprintf( stdout, " Marker spread error.\n" );
			} /* if */
			break;
		} /* if */
	} /* for */

    /*
     * Print out the valid data.
     */
    fprintf( stdout, "\n" );
    fprintf( stdout, "Frame Number: %8u\n", uFrameNumber );
    fprintf( stdout, "Transforms  : %8u\n", uElements );
    fprintf( stdout, "Flags       :   0x%04x\n", uFlags );

    fprintf( stdout, "3D Data Display (%d Markers)\n", dtSettings.nMarkers );
	for( i = 0; i < dtSettings.nMarkers; i++ )
	{
		DisplayMarker( i, pData3d[i] );
	} /* for */
	fprintf( stdout, "\n" );

    fprintf( stdout, "6D Data Display (%d Rigid Bodies)\n", nTotalRigids );
    for( nCurRigid = 0; nCurRigid < nTotalRigids; ++nCurRigid )
    {
        fprintf( stdout, "Rigid Body %u\n", pData6d[nCurRigid].RigidId );
		if( pData6d[nCurRigid].flags & OPTOTRAK_UNDETERMINED_FLAG )
		{
			fprintf( stdout, "MISSING.  Undertermined transform.\n\n" );
		}
		else
		{
			fprintf( stdout, "XT = %8.2f YT = %8.2f ZT = %8.2f\n",
					 pData6d[nCurRigid].transformation.euler.translation.x,
					 pData6d[nCurRigid].transformation.euler.translation.y,
					 pData6d[nCurRigid].transformation.euler.translation.z );
			fprintf( stdout, "Y  = %8.2f P  = %8.2f R  = %8.2f\n\n",
					 pData6d[nCurRigid].transformation.euler.rotation.yaw,
					 pData6d[nCurRigid].transformation.euler.rotation.pitch,
					 pData6d[nCurRigid].transformation.euler.rotation.roll );
		} /* if */
    } /* for */


PROGRAM_COMPLETE:
	fprintf( stdout, "\n" );
	fprintf( stdout, "...TransputerShutdownSystem\n" );
    OptotrakDeActivateMarkers( );
    TransputerShutdownSystem( );

	/*
	 * free all memory
	 */
	if( pdtDeviceHandlesInfo )
	{
		for( i = 0; i < nDevices; i++ )
		{
			AllocateMemoryDeviceHandleProperties( &(pdtDeviceHandlesInfo[i].grProperties), 0 );
		} /* for */
	} /* if */
	AllocateMemoryDeviceHandles( &pdtDeviceHandles, 0 );
	AllocateMemoryDeviceHandlesInfo( &pdtDeviceHandlesInfo, pdtDeviceHandles, 0 );
	free( pData3d );
	free( pData6d );

	exit( 0 );

ERROR_EXIT:
	/*
	 * Indicate that an error has occurred
	 */
	fprintf( stdout, "\nAn error has occurred during execution of the program.\n" );
    if( OptotrakGetErrorString( szNDErrorString, MAX_ERROR_STRING_LENGTH + 1 ) == 0 )
    {
        fprintf( stdout, szNDErrorString );
    } /* if */

	/*
	 * shut down the processors
	 */
	fprintf( stdout, "\n\n...TransputerShutdownSystem\n" );
	OptotrakDeActivateMarkers( );
	TransputerShutdownSystem( );

	/*
	 * free all memory
	 */
	if( pdtDeviceHandlesInfo )
	{
		for( i = 0; i < nDevices; i++ )
		{
			AllocateMemoryDeviceHandleProperties( &(pdtDeviceHandlesInfo[i].grProperties), 0 );
		} /* for */
	} /* if */
	AllocateMemoryDeviceHandles( &pdtDeviceHandles, 0 );
	AllocateMemoryDeviceHandlesInfo( &pdtDeviceHandlesInfo, pdtDeviceHandles, 0 );

	if( pData3d )
	{
		free( pData3d );
	} /* if */

	if( pData6d )
	{
		free( pData6d );
	} /* if */

    exit( 1 );

} /* main */

