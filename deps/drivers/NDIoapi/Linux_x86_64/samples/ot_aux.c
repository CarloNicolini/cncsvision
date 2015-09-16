/*****************************************************************
Name:			ot_aux.c

Description:
	Auxiliary functions for the Optotrak sample programs.
	This file contains commonly used functions for the Optotrak,
	including device detection and memory allocation.

*****************************************************************/

/*****************************************************************
C Library Files Included
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

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


void DisplayFloat( float fFloat )
{
	if( fFloat < MAX_NEGATIVE )
	{
		fprintf( stdout, "%10s%5s", "MISSING", "" );
	}
	else
	{
		fprintf( stdout, "%10.2f%5s", fFloat, "" );
	} /* if */
} /* DisplayFloat */


static void DisplayPosition3d( Position3d dtPosition3d	)
{
	fprintf( stdout, "X" );
	DisplayFloat( dtPosition3d.x );
	fprintf( stdout, "Y " );
	DisplayFloat( dtPosition3d.y );
	fprintf( stdout, "Z " );
	DisplayFloat( dtPosition3d.z );

} /* DisplayPosition3d */


void DisplayMarker( int nMarker, Position3d dtPosition3d )
{
	fprintf( stdout, "Marker_%.3d: ", nMarker );
	DisplayPosition3d( dtPosition3d );
	fprintf( stdout, "\n" );

} /* DisplayMarker */

