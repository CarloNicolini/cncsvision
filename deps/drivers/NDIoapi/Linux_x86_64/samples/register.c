/*****************************************************************
Name:             REGISTER.C

Description:
	Optotrak Register
	Perform function call to register multiple position sensors.
	This is the equivalent of the old 'register' program.

*****************************************************************/

/*****************************************************************
C Library Files Included
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>

/*****************************************************************
ND Library Files Included
*****************************************************************/
#include "ndtypes.h"
#include "ndpack.h"
#include "ndopto.h"


/*****************************************************************
Application Files Included
*****************************************************************/

void main( int argc, unsigned char *argv[] )
{
	char
		*pszData,
        szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];
	RegisterParms
		dtRegisterParms;
	float
		fRMSError;
	int
		i;

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak Register\n\n" );

	/*
	 * initialize the registration parameters
	 */
	sprintf( dtRegisterParms.szRawDataFile, "r#001.dat" );
	sprintf( dtRegisterParms.szRigidBodyFile, "rigid.rig" );
	sprintf( dtRegisterParms.szInputCamFile, CAMFILE_STANDARD );
	sprintf( dtRegisterParms.szOutputCamFile, CAMFILE_REGISTER );
	sprintf( dtRegisterParms.szLogFileName, "register.log" );
	dtRegisterParms.fXfrmMaxError     = 100.;
	dtRegisterParms.fXfrm3dRmsError   = 100.;
	dtRegisterParms.fSpread1          = 0.;
	dtRegisterParms.fSpread2          = 0.;
	dtRegisterParms.fSpread3          = 0.;
	dtRegisterParms.nMinNumberOfXfrms = 3;
	dtRegisterParms.nLogFileLevel     = REG_DETAILED_LOG_FILE;
	dtRegisterParms.bCheckCalibration = FALSE;
	dtRegisterParms.bVerbose          = TRUE;

	/*
	 * parse the command line parameters
	 */
	if( argc < 3 )
	{
		goto ERROR_USAGE;
	} /* if */

	sprintf( dtRegisterParms.szRawDataFile, argv[1] );
	sprintf( dtRegisterParms.szRigidBodyFile, argv[2] );

	for( i = 3; i < argc; i++ )
	{
		if( argv[i][0] != '-' )
		{
			goto ERROR_USAGE;
		} /* if */

		pszData = &argv[i][2];
		switch( argv[i][1] )
		{
		  case 'i':
			sprintf( dtRegisterParms.szInputCamFile, pszData );
			break;

		  case 'o':
			sprintf( dtRegisterParms.szOutputCamFile, pszData );
			break;

		  case 'v':
			if( argv[i][2] == '0' )
			{
				dtRegisterParms.bVerbose = FALSE;
			} /* if */
			break;

		  case 'l':
			dtRegisterParms.nLogFileLevel = atoi( pszData );
			if( dtRegisterParms.nLogFileLevel > REG_DETAILED_LOG_FILE )
			{
				dtRegisterParms.nLogFileLevel = REG_DETAILED_LOG_FILE;
			} /* if */
			break;

		  default:
			goto ERROR_USAGE;
		} /* switch */
	} /* for */

	/*
	 * display registration parameters being used
	 */
	fprintf( stdout, "\nRegistration Parameters:\n" );
	fprintf( stdout, "Raw Data file:          %s\n", dtRegisterParms.szRawDataFile );
	fprintf( stdout, "Rigid Body file:        %s\n", dtRegisterParms.szRigidBodyFile );
	fprintf( stdout, "Input Camera file:      %s\n", dtRegisterParms.szInputCamFile );
	fprintf( stdout, "Output Camera file:     %s\n", dtRegisterParms.szOutputCamFile );
	fprintf( stdout, "Log file:               %s\n", dtRegisterParms.szLogFileName );
	fprintf( stdout, "Max Xfrm Error:         %.2f\n", dtRegisterParms.fXfrmMaxError );
	fprintf( stdout, "Max 3D RMS Xfrm Error:  %.2f\n", dtRegisterParms.fXfrm3dRmsError );
	fprintf( stdout, "Spread 1:               %.2f\n", dtRegisterParms.fSpread1 );
	fprintf( stdout, "Spread 2:               %.2f\n", dtRegisterParms.fSpread2 );
	fprintf( stdout, "Spread 3:               %.2f\n", dtRegisterParms.fSpread3 );
	fprintf( stdout, "Min Number of Xfrms:    %d\n", dtRegisterParms.nMinNumberOfXfrms );
	fprintf( stdout, "Log File level:         %d\n", dtRegisterParms.nLogFileLevel );

	/*
	 * run the registration using the specified parameters
	 */
	if( nOptotrakRegisterSystem( dtRegisterParms, &fRMSError ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	 * indicate program completion
	 */
	fprintf( stdout, "\nRegistration Complete.  RMS Error: %10.4f\n", fRMSError );
	fprintf( stdout, "Camera Parameter File created: %s\n", dtRegisterParms.szOutputCamFile );
	fprintf( stdout, "Detailed information         : %s\n", dtRegisterParms.szLogFileName );

	/*
	 * determine the location of the Optotrak NIF file
	 */
	exit( 0 );

ERROR_USAGE:
	/*
	 * indicate usage for this program
	 */
	fprintf( stdout, "Usage: register RawFile RigFile [-iCamFile] [-oNewCamFile] [-v0] [-ln]\n\n" );
	fprintf( stdout, "This program produces a camera parameter file that allows multiple\n"
					 "position sensors to report positions in a common coordinate system.\n\n" );
	exit( 2 );

ERROR_EXIT:
	/*
	 * Indicate that an error has occurred
	 */
	fprintf( stdout, "\nAn error has occurred during execution of the program.\n" );
    if( OptotrakGetErrorString( szNDErrorString, MAX_ERROR_STRING_LENGTH + 1 ) == 0 )
    {
        fprintf( stdout, szNDErrorString );
    } /* if */
    exit( 1 );

} /* main */

