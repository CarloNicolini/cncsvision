/*****************************************************************
Name:             CERTUS_SAMPLE20.C

Description:
	Optotrak Certus Sample 20

	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- If it finds Wireless strobers, it goes to the wireless mode
	  and saves collection properties to Wireless strobers
    - It asks user to replace Wireless strobers with Wireless transmitter
	  and checks if it was done.
	- Sets number of markers previously detected on Wireless strobers to
	  the Wireless transmitter
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Retrieve frames of real-time 3D data using wireless configuration
	  until the first marker is visible.
	- De-activate the markers.
	- Sets the system back to the wired mode.
	- Disconnect from the Optotrak System.

*****************************************************************/

/*****************************************************************
C Library Files Included
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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


#define SAMPLE_MARKERFREQ	3000.0f
#define SAMPLE_FRAMEFREQ	30.0f
#define SAMPLE_DUTYCYCLE	0.60f
#define SAMPLE_VOLTAGE		7.0f
#define SAMPLE_STREAMDATA	0

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
		nDevices,
		nDeviceMarkers;
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
	int bDone = FALSE;
	char szInput[100];

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
	nDeviceMarkers = 0;
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
	fprintf( stdout, "\nOptotrak Certus sample program #20\n\n" );

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

	bDone = FALSE;
	while( !bDone )
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
		* Count all markers on Wireless strobers
		*/
		nMarkersToActivate = 0;
		for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
		{
			if( pdtDevices[nCurDevice].nDeviceType == DH_DEVICE_TYPE_SMART_MARKER_CONTROLLER )
			{
				// at least one wireless strober was found
				bDone = TRUE;
				nMarkersToActivate += pdtDevices[nCurDevice].nMarkersToActivate;
			}
			else if( pdtDevices[nCurDevice].nDeviceType == DH_DEVICE_TYPE_TRANSMITTER_CONTROLLER )
			{
				// set 0 markers to activate on the wireless transmitter for now
				SetMarkersToActivateForDevice( &(pdtDevices[nCurDevice]), pdtDeviceHandlesInfo[nCurDevice].pdtHandle->nID, 0 );
			}
		} /* if */

		if( !bDone )
		{
			fprintf( stdout, "Please connect at least one Wireless Strober. Press ENTER key when ready....\n" );
			gets(szInput);
		}
		else if( nMarkersToActivate == 0 )
		{
		   /*
			* Ensure that we are firing some markers
			*/
			bDone = FALSE;
			fprintf( stdout, "Error: There are no markers to be activated. Plug some markers in Wireless Strober.\nPress ENTER key when ready....\n" );
			gets(szInput);
		} /* if */
	}// while
	fprintf( stdout, "\n" );


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
     * setting the wireless mode for the system so that when we later save collection settings
	 * to Wireless strobers they are already setup for wireless mode
	 */
	fprintf( stdout, "...OptotrakEnableWireless  enabling wireless mode\n" );
	if( OptotrakEnableWireless( -1, 1 ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	}

    /*
     * save collection settings to Wireless strobers
	 */
	fprintf( stdout, "...Saving collection parameters to Wireless strobers\n" );
    if( OptotrakSetupCollection( nMarkersToActivate,
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
     * ask to replace Wireless strobers with Wireless transmitter
	 */
	fprintf(stdout, "Please remove Wireless strobers and connect them to batteries.\n" );
	bDone = FALSE;
	while( !bDone )
	{
		fprintf(stdout, "Connect only Wireless transmitter to the SCU.\nPress ENTER key when ready ...");
		gets(szInput);
		fprintf(stdout, "\n");

		/*
		 * check if it was done
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
			fprintf( stdout, ".........no devices detected.\n" );
			bDone = FALSE;
		} /* if */
		else
		{
			/*
			* Now that all the device handles have been completely set up,
			* the application can store all the device handle information in
			* an internal data structure.  This will facilitate lookups when
			* a property setting needs to be checked.
			*/
			ApplicationStoreDeviceProperties( &pdtDevices, pdtDeviceHandlesInfo, nDevices );

			if( nDevices > 1 )
			{
				bDone = FALSE;
				fprintf(stdout, "Please connect only Wireless Transmitter to the SCU.\nPress ENTER key when ready ...");
				gets(szInput);
				fprintf(stdout, "\n");
				bDone = FALSE;
			}
			else if ( pdtDevices[0].nDeviceType == DH_DEVICE_TYPE_TRANSMITTER_CONTROLLER )
			{
				/*
				 * Now when we know the user has switched Wireless Strobers for Wireless Transmitter
				 * we set number of markers previously detected in Wireless Strobers to the Wireless Transmitter
				 */
				fprintf( stdout, "Setting %d markers to be fired in Wireless Transmitter\n", nMarkersToActivate );
				SetMarkersToActivateForDevice( &(pdtDevices[0]), pdtDeviceHandlesInfo[0].pdtHandle->nID, nMarkersToActivate );
				bDone = TRUE;
			}
			else
			{
				bDone = FALSE;
			}
		}
	}// while

		/*
	 * Determine the collection settings based on the device properties
	 */
	ApplicationDetermineCollectionParameters( nDevices, pdtDevices, &dtSettings );

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
     * Retrieve a frame of 3D data.
     * Use the blocking 'GetLatest' call to retrieve a frame of data.
     * All the data retrieveal functions return status information in
     * the uFlags field.
     * If the flags indicate that the system configuration has changed,
     * all processing should stop and the application should re-query
     * the system for all device information.
     * NOTE: The re-initialization is not done here in order to
     *       simplify the sample program.
     */
	do
	{
		if( DataGetLatest3D( &uFrameNumber, &uElements, &uFlags, p3dData ) != OPTO_NO_ERROR_CODE )
		{
			goto ERROR_EXIT;
		} /* if */

		/*
		* display the 3d data
		*/
		fprintf( stdout, "\n\nSample Program Results:\n\n" );
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
		sleep(1);
	}while( p3dData[0].x < MAX_NEGATIVE  );

    /*
     * De-activate the markers.
     */
	fprintf( stdout, "...OptotrakDeActivateMarkers\n" );
    if( OptotrakDeActivateMarkers() )
    {
        goto ERROR_EXIT;
    } /* if */

    /*
     * setting the wired mode for the system
	 */
	fprintf( stdout, "...OptotrakEnableWireless  disabling wireless mode\n" );
	if( OptotrakEnableWireless( -1, 0 ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	}

PROGRAM_COMPLETE:
    /*
     * CLEANUP
     */
	fprintf( stdout, "\n" );
	fprintf( stdout, "...TransputerShutdownSystem\n" );
    TransputerShutdownSystem( );
	fprintf( stdout, "Press ENTER key to finish....\n" );
	gets(szInput);

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

