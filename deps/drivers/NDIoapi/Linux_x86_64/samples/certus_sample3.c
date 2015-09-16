/*****************************************************************
Name:             CERTUS_SAMPLE3.C

Description:
 	Optotrak Certus Sample 3

	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Retrieve the strober properties and display the number of properties.
	- Retrieve/display the property 'PORT' for each strober.
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

#define HANDLE_IDX			0


/*****************************************************************
Application Files Included
*****************************************************************/
#include "certus_aux.h"


void main( int argc, unsigned char *argv[] )
{
	OptotrakSettings
		dtSettings;
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];
    int
    	nCurDevice,
		nDevices,
		nSwitches;
	ApplicationDeviceInformation
		*pdtDevices;
    DeviceHandle
        *pdtDeviceHandles;
    DeviceHandleInfo
        *pdtDeviceHandlesInfo;
	DeviceHandleProperty
		dtProperty;


    /*
     * initialization
	 * intialize variables
     */
	pdtDevices = NULL;
	pdtDeviceHandles = NULL;
	pdtDeviceHandlesInfo = NULL;
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
	nSwitches = 0;

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak Certus sample program #3\n\n" );

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

	if( nDevices > 0 )
	{
		/*
		 * Get number of properties
		 */
		fprintf( stdout, "...OptotrakDeviceHandleGetNumberProperties\n" );
		for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
		{
			fprintf( stdout, "\n" );
			fprintf( stdout, "...Device %d: \n",  (nCurDevice + 1) );
			if( OptotrakDeviceHandleGetNumberProperties( pdtDeviceHandlesInfo[ nCurDevice ].pdtHandle->nID,
														&pdtDeviceHandlesInfo[ nCurDevice ].nProperties ) != OPTO_NO_ERROR_CODE )
			{
				goto ERROR_EXIT;
			} /* if */
			else
			{
				fprintf( stdout, "......Number of Properties = %d\n", pdtDeviceHandlesInfo[ nCurDevice ].nProperties );
			} /* else */

			/*
			 * Get property "DH_PROPERTY_PORT"
			 */
			if( OptotrakDeviceHandleGetProperty( pdtDeviceHandlesInfo[ nCurDevice ].pdtHandle->nID,
												 &dtProperty,
												 DH_PROPERTY_PORT ) != OPTO_NO_ERROR_CODE )
			{
				goto ERROR_EXIT;
			} /* if */
			else
			{
				fprintf( stdout, "......Property-Port = %d\n", dtProperty.dtData.nData );
			} /* else */
		} /* for */
	}
	else
	{
		fprintf( stdout, "...No device has been found, check your system and try again\n" );
	}/* else */




PROGRAM_COMPLETE:
    /*
     * CLEANUP
     */
	fprintf( stdout, "\n" );
	fprintf( stdout, "...TransputerShutdownSystem\n" );
    OptotrakDeActivateMarkers( );
    TransputerShutdownSystem( );
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

    exit( 1 );

} /* main */

