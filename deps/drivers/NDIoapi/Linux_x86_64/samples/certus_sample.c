/*****************************************************************
Name:             CERTUS_SAMPLE.C

Description:
 	Optotrak Certus Sample

	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Set number of markers to activate on each strober.
	- Retrieve/display device properties.
	- Set processing flags to perform data conversions on the host computer.
	- Load the appropriate camera parameters.
	- Set up an Optotrak collection.
	- Activate the markers.
	- Request/receive/display 5 frames of real-time 3D data.
	- De-activate the markers.
	- Stop the Optotrak collection.
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
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1],
		szProperty[32];
    int
		i,
		nCurDevice,
		nCurProperty,
		nCurFrame,
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
	char
		*pChar,
		szInput[10];


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


EXECUTION_START:
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
	} /* if */
	fprintf( stdout, "\n" );

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

	/*
	 * Display all the property values
	 */
	for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
	{
		fprintf( stdout, "...Device %d properties:\n", nCurDevice );
		for( nCurProperty = 0; nCurProperty < pdtDeviceHandlesInfo[nCurDevice].nProperties; nCurProperty++ )
		{
			/*
			 * Determine the property
			 */
			switch( pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].uPropertyID )
			{
			case DH_PROPERTY_NAME:
				sprintf( szProperty, "Device Name          " );
				break;

			case DH_PROPERTY_MARKERSTOFIRE:
				sprintf( szProperty, "Markers To Activate  " );
				/*
				 * add to the number of total markers being fired
				 */
				dtSettings.nMarkers += pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].dtData.nData;
				break;

			case DH_PROPERTY_MAXMARKERS:
				sprintf( szProperty, "MaxDeviceMarkers     " );
				break;

			case DH_PROPERTY_STARTMARKERPERIOD:
				sprintf( szProperty, "Start Marker Period  " );
				break;

			case DH_PROPERTY_SWITCHES:
				sprintf( szProperty, "Device Switches      " );
				break;

			case DH_PROPERTY_VLEDS:
				sprintf( szProperty, "Device VLEDs         " );
				break;

			case DH_PROPERTY_PORT:
				sprintf( szProperty, "Strober Port         " );
				break;

			case DH_PROPERTY_ORDER:
				sprintf( szProperty, "Strober Port Order   " );
				break;

			case DH_PROPERTY_SUBPORT:
				sprintf( szProperty, "Strober SubPort      " );
				break;

			case DH_PROPERTY_FIRINGSEQUENCE:
				sprintf( szProperty, "Activation Sequence  " );
				break;

			case DH_PROPERTY_HAS_ROM:
				sprintf( szProperty, "Has Rom?             " );
				break;

			case DH_PROPERTY_TOOLPORTS:
				sprintf( szProperty, "Num ToolPorts        " );
				break;

			case DH_PROPERTY_3020_CAPABILITY:
				sprintf( szProperty, "3020 Capability      " );
				break;

			case DH_PROPERTY_3020_MARKERSTOFIRE:
				sprintf( szProperty, "3020 MrkrsToActivate " );
				/*
				 * add to the number of total markers being fired
				 */
				dtSettings.nMarkers += pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].dtData.nData;
				break;

			case DH_PROPERTY_3020_STARTMARKERPERIOD:
				sprintf( szProperty, "3020 StartMarkerPrd  " );
				break;

			case DH_PROPERTY_STATUS:
				sprintf( szProperty, "Status               " );
				break;

			case DH_PROPERTY_SROM_RIGIDMARKERS:
				sprintf( szProperty, "SROM Rigid Markers   " );
				break;

			case DH_PROPERTY_SROM_RIGIDPOSITIONS:
				sprintf( szProperty, "SROM Rigid Markers   " );
				break;

			case DH_PROPERTY_SROM_FIRINGSEQUENCE:
				sprintf( szProperty, "SROM Firing Sequence " );
				break;

			case DH_PROPERTY_SROM_NORMALMARKERS:
				sprintf( szProperty, "SROM Normal Markers  " );
				break;

			case DH_PROPERTY_SROM_NORMALS:
				sprintf( szProperty, "SROM Normals         " );
				break;

			case DH_PROPERTY_SROM_SERIALNUMBER:
				sprintf( szProperty, "SROM Serial Number   " );
				break;

			case DH_PROPERTY_SROM_PARTNUMBER:
				sprintf( szProperty, "SROM Part Number     " );
				break;

			case DH_PROPERTY_SROM_MANUFACTURER:
				sprintf( szProperty, "SROM Manufacturer    " );
				break;

			case DH_PROPTERY_SROM_TOOLTYPE:
				sprintf( szProperty, "SROM Tool Type       " );
				break;

			case DH_PROPERTY_STATE_FLAGS:
				sprintf( szProperty, "State Flags          " );
				break;

			case DH_PROPERTY_TIP_ID:
				sprintf( szProperty, "TIP Id               " );
				break;

			case DH_PROPERTY_SCAN_MODE_SUPPORT:
				sprintf( szProperty, "Scan Mode Support?   " );
				break;

			case DH_PROPERTY_HAS_BUMP_SENSOR:
				sprintf( szProperty, "Has Bump Sensor?     " );
				break;

			case DH_PROPERTY_WIRELESS_ENABLED:
				sprintf( szProperty, "Wireless Enabled?    " );
				break;

			case DH_PROPERTY_HAS_BEEPER:
				sprintf( szProperty, "Has Beeper?          " );
				break;

			case DH_PROPERTY_DEVICE_TYPE:
				sprintf( szProperty, "Device Type          " );
				break;

			case DH_PROPERTY_SERIAL_NUMBER:
				sprintf( szProperty, "Serial Number        " );
				break;

			case DH_PROPERTY_FEATURE_KEY:
				sprintf( szProperty, "Feature Key          " );
				break;

			case DH_PROPERTY_MKR_PORT_ENCODING:
				sprintf( szProperty, "Marker Port Encoding " );
				break;

			case DH_PROPERTY_SMART_MARKERS:
				sprintf( szProperty, "Smart Mkr Support?   " );
				break;

			case DH_PROPERTY_UNKNOWN:
			default:
				sprintf( szProperty, "Unknown Property %.2d  ", pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].uPropertyID );
				break;

			} /* switch */

			/*
			 * Determine the property's data type before displaying it
			 */
			switch( pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].dtPropertyType )
			{
				case DH_PROPERTY_TYPE_INT:
					fprintf( stdout, "......Property_%.2d - %s = %d\n", nCurProperty, szProperty, pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].dtData.nData );
					break;
				case DH_PROPERTY_TYPE_FLOAT:
					fprintf( stdout, "......Property_%.2d - %s = %f\n", nCurProperty, szProperty, pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].dtData.fData );
					break;
				case DH_PROPERTY_TYPE_DOUBLE:
					fprintf( stdout, "......Property_%.2d - %s = %f\n", nCurProperty, szProperty, pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].dtData.dData );
					break;
				case DH_PROPERTY_TYPE_CHAR:
					fprintf( stdout, "......Property_%.2d - %s = %s\n", nCurProperty, szProperty, pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].dtData.cData );
					break;
				case DH_PROPERTY_TYPE_LONG:
					fprintf( stdout, "......Property_%.2d - %s = %ld\n", nCurProperty, szProperty, pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].dtData.lData );
					break;
				case DH_PROPERTY_TYPE_STRING:
					if( pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].uPropertyID == DH_PROPERTY_FIRINGSEQUENCE )
					{
						fprintf( stdout, "......Property_%.2d - %s = ", nCurProperty, szProperty );
						for( pChar = pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].dtData.szData; ( pChar != '\0' ) && ( *pChar != 0 ); pChar++ )
						{
							fprintf( stdout, "%.3d, ", *pChar );
						} /* for */
						fprintf( stdout, "\n" );
					}
					else
					{
						fprintf( stdout, "......Property_%.2d - %s = %s\n", nCurProperty, szProperty, pdtDeviceHandlesInfo[nCurDevice].grProperties[nCurProperty].dtData.szData );
					} /* if */
					break;
				default:
					fprintf( stdout, "......Property_%.2d = UNKNOWN PROPERTY TYPE\n", nCurDevice );
					break;
			} /* switch */

		} /* for */

		fprintf( stdout, "\n" );
	} /* for */


	fprintf( stdout, "Continue?  or determine strober configuration Again? [c|a] : " );
	gets( szInput );
	if( szInput[0] == 'a' )
	{
		goto EXECUTION_START;
	} /* if */

	/*
	 * Retrieve Optotrak Data
	 */

	fprintf( stdout, "\n" );

	/*
	 * check if any devices have been detected by the system
	 */
	if( nDevices == 0 )
	{
		fprintf( stdout, ".........no devices detected.\n" );
		fprintf( stdout, "Quit program? [y|n] : " );
		gets( szInput );
		if( szInput[0] == 'y' )
		{
			goto PROGRAM_COMPLETE;
		} /* if */

		fprintf( stdout, "...restarting program execution\n\n" );
		goto EXECUTION_START;
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
    fprintf( stdout, "\n\nSample Program Results:\n\n" );
    fprintf( stdout, "\n\n3D Data Display\n" );
    for( nCurFrame = 0; nCurFrame < 5; nCurFrame++ )
    {
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
	 * Stop the collection.
	 */
	fprintf( stdout, "...OptotrakStopCollection\n" );
	if( OptotrakStopCollection( ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */

	fprintf( stdout, "Run again? [y|n] : " );
	gets( szInput );
	if( szInput[0] == 'y' )
	{
		goto EXECUTION_START;
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

