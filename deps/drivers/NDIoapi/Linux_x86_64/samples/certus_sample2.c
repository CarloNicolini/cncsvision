/*****************************************************************
Name:             CERTUS_SAMPLE2.C

Description:
 	Optotrak Certus Sample 2

	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set number of markers to activate on each strober.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 3D data until the strober configuration changes.
	- De-activate the markers.
	- Stop the Optotrak collection.
	- Disconnect from the Optotrak System.
	- Re-start collection and wait until configuration changes again.
	- Receive latest strober configuration.

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

#define CERTUS_SAMPLE_STROBER_MARKERSTOFIRE			2
#define CERTUS_SAMPLE_3020_STROBER_MARKERSTOFIRE	6

/*****************************************************************
Application Files Included
*****************************************************************/
#include "certus_aux.h"
#include "ot_aux.h"


void main( int argc, unsigned char *argv[] )
{
	OptotrakSettings
		dtSettings;
    int
		i,
		nIterations,
		nCurDevice,
		nCurMarker,
		nDevices,
		nMarkersToActivate;
	ApplicationDeviceInformation
		*pdtDevices;
    DeviceHandle
        *pdtDeviceHandles;
    DeviceHandleInfo
        *pdtDeviceHandlesInfo;
	unsigned int
		uFlags,
		uElements,
		uFrameNumber;
    Position3d
        *p3dData;
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];


    /*
     * initialization
	 * intialize variables
     */
	pdtDevices = NULL;
	pdtDeviceHandles = NULL;
	pdtDeviceHandlesInfo = NULL;
	p3dData = NULL;
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
	nMarkersToActivate = 0;

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak Certus sample program #2\n\n" );

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
	 * This sample is intended for Optotrak Certus systems.
	 */
	fprintf( stdout, "...DetermineSystem\n" );
	if( uDetermineSystem( ) != OPTOTRAK_CERTUS_FLAG )
	{
		goto ERROR_EXIT;
	} /* if */

	nIterations = 2;
	for( i = 0; i < nIterations; i++ )
	{
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
		 * Change the number of markers to fire for all devices
		 */
		for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
		{
			nMarkersToActivate = pdtDevices[nCurDevice].b3020Capability?
								   CERTUS_SAMPLE_3020_STROBER_MARKERSTOFIRE : CERTUS_SAMPLE_STROBER_MARKERSTOFIRE;

			SetMarkersToActivateForDevice( &(pdtDevices[nCurDevice]), pdtDeviceHandlesInfo[nCurDevice].pdtHandle->nID, nMarkersToActivate );
		} /* for */
		fprintf( stdout, "\n" );

		/*
		 * If there are strobers with tool capabilities, but no tools,
		 * then set up the strobers to fire some markers
		 */
		for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
		{
			if( pdtDevices[nCurDevice].nToolPorts > 0 )
			{
				/*
				 * set the markers to activate for this device
				 * this will be 0 if there are tools plugged into the
				 * device, or CERTUS_SAMPLE_STROBER_MARKERSTOFIRE if there
				 * are no tools plugged in
				 */
				nMarkersToActivate = CERTUS_SAMPLE_STROBER_MARKERSTOFIRE;
				SetMarkersToActivateForToolDevice( &(pdtDevices[nCurDevice]), pdtDeviceHandlesInfo[nCurDevice].pdtHandle->nID, nMarkersToActivate, pdtDevices, nDevices );
			} /* if */
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
		 * Load camera parameters.
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
		 * allocate memory for 3d data
		 */
		p3dData = (Position3d*)malloc( dtSettings.nMarkers * sizeof( Position3d ) );

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

		/*
		 * Wait one second to let the camera adjust.
		 */
		sleep( 1 );

		/*
		 * Prepare for realtime data retrieval.
		 * Activate markers. Turn on the markers prior to data retrieval.
		 */
		fprintf( stdout, "...OptotrakActivateMarkers\n" );
		if( OptotrakActivateMarkers( ) != OPTO_NO_ERROR_CODE )
		{
			goto ERROR_EXIT;
		} /* if */
		sleep( 1 );

		/*
		 * loop around until a device is plugged or unplugged
		 */
		while( 1 )
		{
			fprintf( stdout, "\nCHANGE THE STROBER CONFIGURATION TO END THIS TEST\n\n" );
			sleep( 1 );

			if( DataGetLatest3D( &uFrameNumber, &uElements, &uFlags, p3dData ) != OPTO_NO_ERROR_CODE )
			{
				goto ERROR_EXIT;
			} /* if */

			/*
			 * display the 3d data
			 */
			fprintf( stdout, "3D Data Display (%d Markers)\n", dtSettings.nMarkers );
			/*
			 * Print out the data.
			 */
			fprintf( stdout, "Frame Number: %8u\n", uFrameNumber );
			fprintf( stdout, "Elements    : %8u\n", uElements );
			fprintf( stdout, "Flags       : 0x%04x\n", uFlags );
			for( nCurMarker = 0; nCurMarker < dtSettings.nMarkers; nCurMarker++ )
			{
				DisplayMarker( nCurMarker + 1, p3dData[nCurMarker] );
			} /* for */

			/*
			 * check if the tool configuration has changed
			 */
			if( uFlags & OPTO_TOOL_CONFIG_CHANGED_FLAG )
			{
				fprintf( stdout, "\n\n\n...Tool Configuration Changed\n" );
				sleep( 3 );
				break;
			} /* if */
		} /* while */

		sleep( 1 );

		/*
		 * stop the collection since we are finished at this point
		 */
		fprintf( stdout, "...OptotrakStopCollection\n\n\n" );
		if( OptotrakStopCollection( ) != OPTO_NO_ERROR_CODE )
		{
			goto ERROR_EXIT;
		} /* if */

	} /* for */

	/*
	 * Exiting the loop above means that the tool configuration has changed
	 */
	fprintf( stdout, "...DetermineStroberConfiguration\n" );
	if( DetermineStroberConfiguration( &pdtDeviceHandles, &pdtDeviceHandlesInfo, &nDevices ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */


PROGRAM_COMPLETE:
    /*
     * CLEANUP
     */
	fprintf( stdout, "\n" );
	fprintf( stdout, "...TransputerShutdownSystem\n" );
    OptotrakDeActivateMarkers( );
    TransputerShutdownSystem( );

	/*
	 * free all memory
	 */
	if( pdtDeviceHandlesInfo )
	{
		for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
		{
			AllocateMemoryDeviceHandleProperties( &(pdtDeviceHandlesInfo[nCurDevice].grProperties), 0 );
		} /* for */
	} /* if */
	AllocateMemoryDeviceHandles( &pdtDeviceHandles, 0 );
	AllocateMemoryDeviceHandlesInfo( &pdtDeviceHandlesInfo, pdtDeviceHandles, 0 );

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

	fprintf( stdout, "\n\n...TransputerShutdownSystem\n" );
	OptotrakDeActivateMarkers( );
	TransputerShutdownSystem( );

	/*
	 * free all memory
	 */
	if( pdtDeviceHandlesInfo )
	{
		for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
		{
			AllocateMemoryDeviceHandleProperties( &(pdtDeviceHandlesInfo[nCurDevice].grProperties), 0 );
		} /* for */
	} /* if */
	AllocateMemoryDeviceHandles( &pdtDeviceHandles, 0 );
	AllocateMemoryDeviceHandlesInfo( &pdtDeviceHandlesInfo, pdtDeviceHandles, 0 );

    exit( 1 );

} /* main */

