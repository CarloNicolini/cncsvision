/*****************************************************************
Name:             ALIGN.C

Description:
	Optotrak Align
	Perform function call to align to new coordinate system.
	This is the equivalent of the old 'align' program.

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
	AlignParms
		dtAlignParms;
	float
		fRMSError;
	int
		i;

	/*
	 * Announce that the program has started
	 */
	fprintf( stdout, "\nOptotrak Align\n\n" );

	/*
	 * initialize the alignment parameters
	 */
	sprintf( dtAlignParms.szDataFile, "c#001.dat" );
	sprintf( dtAlignParms.szRigidBodyFile, "rigid.rig" );
	sprintf( dtAlignParms.szInputCamFile, CAMFILE_STANDARD );
	sprintf( dtAlignParms.szOutputCamFile, CAMFILE_ALIGN );
	sprintf( dtAlignParms.szLogFileName, "align.log" );
	dtAlignParms.nLogFileLevel = ALIGN_DETAILED_LOG_FILE;
	dtAlignParms.bVerbose      = TRUE;

	/*
	 * parse the command line parameters
	 */
	if( argc < 3 )
	{
		goto ERROR_USAGE;
	} /* if */

	sprintf( dtAlignParms.szDataFile, argv[1] );
	sprintf( dtAlignParms.szRigidBodyFile, argv[2] );

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
			sprintf( dtAlignParms.szInputCamFile, pszData );
			break;

		  case 'o':
			sprintf( dtAlignParms.szOutputCamFile, pszData );
			break;

		  case 'v':
			if( argv[i][2] == '0' )
			{
				dtAlignParms.bVerbose = FALSE;
			} /* if */
			break;

		  default:
			goto ERROR_USAGE;
		} /* switch */
	} /* for */

	/*
	 * display alignment parameters being used
	 */
	fprintf( stdout, "\nAlignment Parameters:\n" );
	fprintf( stdout, "Input Data file:        %s\n", dtAlignParms.szDataFile );
	fprintf( stdout, "Rigid Body file:        %s\n", dtAlignParms.szRigidBodyFile );
	fprintf( stdout, "Input Camera file:      %s\n", dtAlignParms.szInputCamFile );
	fprintf( stdout, "Output Camera file:     %s\n", dtAlignParms.szOutputCamFile );
	fprintf( stdout, "Log file:               %s\n", dtAlignParms.szLogFileName );
	fprintf( stdout, "Log File level:         %d\n", dtAlignParms.nLogFileLevel );

	/*
	 * run the alignment using the specified parameters
	 */
	if( nOptotrakAlignSystem( dtAlignParms, &fRMSError ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	 * indicate program completion
	 */
	fprintf( stdout, "\nAlignment Complete.  RMS Error: %10.4f\n", fRMSError );
	fprintf( stdout, "Camera Parameter File created: %s\n", dtAlignParms.szOutputCamFile );
	fprintf( stdout, "Detailed information         : %s\n", dtAlignParms.szLogFileName );

	/*
	 * determine the location of the Optotrak NIF file
	 */
	exit( 0 );

ERROR_USAGE:
	/*
	 * indicate usage for this program
	 */
	fprintf( stdout, "Usage: align DataFile RigFile [-iCamFile] [-oNewCamFile] [-v0]\n\n" );
	fprintf( stdout, "This program produces a camera parameter file that allows multiple\n"
					 "position sensors to report positions in a user determined frame of\n"
					 "reference.\n" );

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

