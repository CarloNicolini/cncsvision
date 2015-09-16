/*****************************************************************
Name:             SLEEP.C

Description:

    An implementation of the function 'sleep' for Microsoft C/C++
    compilers, which do not include it in their standard library.

*****************************************************************/

/*****************************************************************
C Library Files Included
*****************************************************************/
#include <stdio.h>
#include <time.h>

/*****************************************************************
Name:               main

Input Values:
    unsigned int
        uSec        :Number of seconds to delay.

Output Values:
	None.

Return Value:
	None.

Description:
    Delay processing for the input number of seconds.

*****************************************************************/

void sleep( unsigned int uSec )
{
   clock_t end;

   end  = uSec * CLOCKS_PER_SEC + clock();
   while( clock() < end )
      ;
}


