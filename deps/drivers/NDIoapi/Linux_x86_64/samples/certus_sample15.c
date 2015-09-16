/*****************************************************************
Name:             CERTUS_SAMPLE15.C

Description:
	Optotrak Certus Sample 15

	- Initiate communications with the Optotrak System.
	- Determine SCU version.
	- Determine strober configuration.
	- Request new name for each strober.
	- Set the name in memory for each strober.
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
#include "certus_aux.h"


void main( int argc, unsigned char *argv[] )
{
    char
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];
	ApplicationDeviceInformation
		*pdtDevices;
    DeviceHandle
        *pdtDeviceHandles;
    DeviceHandleInfo
        *pdtDeviceHandlesInfo;
	int
		i,
		nCurDevice,
		nDevices;
	char
		szInput[128];
	DeviceHandleProperty
		dtProperty;


    /*
     * initialization
	 * intialize variables
     */
	pdtDevices = NULL;
	pdtDeviceHandles = NULL;
	pdtDeviceHandlesInfo = NULL;

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak Certus sample program #15\n\n" );

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
	 * Request a new name for the strober device
	 */
	for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
	{

		fprintf( stdout, "\n" );
		fprintf( stdout, "Device Found with name : %s\n", pdtDevices[nCurDevice].szName );
		/*
		 * do not allow changes to devices with SROMs
		 */
		if( pdtDevices[nCurDevice].bHasROM )
		{
			fprintf( stdout, "...device contains SROM.  Name cannot be changed.\n" );
			continue;
			szInput[0] = 's';
		}
		else
		{
			fprintf( stdout, "...enter new name for this device ('s' to Skip renaming) : " );
			gets( szInput );
		} /* if */

		if( ( szInput[0] == 's' ) && ( szInput[1] == '\0' ) )
		{
			fprintf( stdout, "...skipping\n" );
			continue;
		} /* if */

		/*
		 * Set the name for the device
		 */
		dtProperty.uPropertyID = DH_PROPERTY_NAME;
		dtProperty.dtPropertyType = DH_PROPERTY_TYPE_STRING;
		sprintf( dtProperty.dtData.szData, szInput );
		fprintf( stdout, "...OptotrakDeviceHandleSetProperties\n" );
		if( OptotrakDeviceHandleSetProperties( pdtDeviceHandlesInfo[nCurDevice].pdtHandle->nID,
											   &dtProperty, 1 ) != OPTO_NO_ERROR_CODE )
		{
			goto ERROR_EXIT;
		} /* if */

		memset( &dtProperty, 0, sizeof( DeviceHandleProperty ) );

		/*
		 * read back the device name
		 */
		fprintf( stdout, "...OptotrakDeviceHandleGetProperty\n" );
		if( OptotrakDeviceHandleGetProperty( pdtDeviceHandlesInfo[nCurDevice].pdtHandle->nID,
											 &dtProperty, DH_PROPERTY_NAME ) != OPTO_NO_ERROR_CODE )
		{
			goto ERROR_EXIT;
		} /* if */

		fprintf( stdout, "......new device name : %s\n", dtProperty.dtData.szData );

	} /* if */
	fprintf( stdout, "\n" );


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

    exit( 1 );

} /* main */

