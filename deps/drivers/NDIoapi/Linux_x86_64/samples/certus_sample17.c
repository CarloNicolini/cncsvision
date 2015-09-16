/*****************************************************************
Name:             CERTUS_SAMPLE17.C

Description:
 	Optotrak Certus Sample 17

	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration until configuration is changed.
	- Set number of markers to activate on each strober.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Retrieve/display 1 frame of 3D data.
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

#define CERTUS_SAMPLE_STROBER_MARKERSTOFIRE		2
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
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];
    int
		i,
		nCurDevice,
		nCurMarker,
		nMarkersToActivate,
		nOriginalDevices,
		nDevices;
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
	boolean
		bFirstTime = TRUE;

    /*
     * initialization
	 * intialize variables
     */
	pdtDevices = NULL;
	pdtDeviceHandles = NULL;
	pdtDeviceHandlesInfo = NULL;
	p3dData = NULL;
	nMarkersToActivate = 0;
	nDevices = 0;
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

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak Certus sample program\n\n" );

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
		goto PROGRAM_COMPLETE;
	} /* if */


	while( 1 )
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

		if( bFirstTime )
		{
			nOriginalDevices = nDevices;
			bFirstTime = FALSE;
		} /* if */

		/*
		 * Now that all the device handles have been completely set up,
		 * the application can store all the device handle information in
		 * an internal data structure.  This will facilitate lookups when
		 * a property setting needs to be checked.
		 */
		ApplicationStoreDeviceProperties( &pdtDevices, pdtDeviceHandlesInfo, nDevices );

		/*
		 * Display the device names
		 */
		fprintf( stdout, "...Devices Found:\n" );
		for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
		{
			fprintf( stdout, ".........Device %.2d: %s\n", nCurDevice, pdtDevices[nCurDevice].szName );
		} /* for */

		if( nDevices != nOriginalDevices )
		{
			fprintf( stdout, "\nDEVICE STATUS CHANGE DETECTED\n" );
			break;
		} /* if */

		fprintf( stdout, "\nCHANGE DEVICE STATUS TO EXIT THIS LOOP\n" );
		sleep( 2 );

	} /* while */

	/*
	 * Change the number of markers to fire for all devices
	 */
	for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
	{
		nMarkersToActivate = pdtDevices[nCurDevice].b3020Capability?
							   CERTUS_SAMPLE_3020_STROBER_MARKERSTOFIRE : CERTUS_SAMPLE_STROBER_MARKERSTOFIRE;

		SetMarkersToActivateForDevice( &(pdtDevices[nCurDevice]), pdtDeviceHandlesInfo[nCurDevice].pdtHandle->nID, nMarkersToActivate );
	} /* if */

	/*
	 * Retrieve the device properties again to verify that the changes took effect.
	 */
	for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
	{
		if( GetDevicePropertiesFromSystem( &(pdtDeviceHandlesInfo[nCurDevice]) ) != OPTO_NO_ERROR_CODE )
		{
			goto ERROR_EXIT;
		} /* if */
	} /* for */

	if( ApplicationStoreDeviceProperties( &pdtDevices, pdtDeviceHandlesInfo, nDevices ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */
	fprintf( stdout, "\n" );

	/*
	 * check if any devices have been detected by the system
	 */
	if( nDevices == 0 )
	{
		goto PROGRAM_COMPLETE;
	} /* if */

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
     * Get and display five frames of 3D data.
     */
    fprintf( stdout, "\n3D Data Display\n" );
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
    for( nCurMarker = 0; nCurMarker < dtSettings.nMarkers; nCurMarker++ )
    {
		DisplayMarker( nCurMarker + 1, p3dData[nCurMarker] );
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


PROGRAM_COMPLETE:
    /*
     * CLEANUP
     */
	fprintf( stdout, "\n" );
	fprintf( stdout, "...TransputerShutdownSystem\n" );
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
	free( p3dData );

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
		for( i = 0; i < nDevices; i++ )
		{
			AllocateMemoryDeviceHandleProperties( &(pdtDeviceHandlesInfo[i].grProperties), 0 );
		} /* for */
	} /* if */
	AllocateMemoryDeviceHandles( &pdtDeviceHandles, 0 );
	AllocateMemoryDeviceHandlesInfo( &pdtDeviceHandlesInfo, pdtDeviceHandles, 0 );
	free( p3dData );

    exit( 1 );

} /* main */

