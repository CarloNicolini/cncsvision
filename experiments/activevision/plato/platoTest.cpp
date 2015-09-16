// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2011 Carlo Nicolini <carlo.nicolini@iit.it>
//
// CNCSVision is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CNCSVision is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// CNCSVision. If not, see <http://www.gnu.org/licenses/>.

#ifdef XXXX
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <Eigen/Core>
#include <direct.h>
#include <stdio.h>
#include <conio.h>

#ifdef _WIN32
#include <windows.h>
#include <MMSystem.h>
#endif

#define PPORT_BASE 0x378 // this is the port's signal pin
/************************ PLATO I/O STUFF ***************************/
/* Definitions in the build of inpout32.dll are:            */
/*   short _stdcall Inp32(short PortAddress);               */
/*   void _stdcall Out32(short PortAddress, short data);    */
/* prototype (function typedef) for DLL function Inp32: */
typedef short (_stdcall *inpfuncPtr)(short portaddr);
typedef void (_stdcall *oupfuncPtr)(short portaddr, short datum);

// Prototypes for Plato functions
int plato_init(void);
int plato_stop(void);
void plato_read(void);
void plato_write(short datum);

// Handle for the inpout32 library
HINSTANCE hLib;// = LoadLibrary("inpout32.dll");

// After successful init, these 2 variables will contain function pointers.*/
inpfuncPtr inp32fp;
oupfuncPtr oup32fp;

// Wrapper functions for the function pointers - call these to perform I/O. */
short  Inp32 (short portaddr){return (inp32fp)(portaddr);}
void  Out32 (short portaddr, short datum){(oup32fp)(portaddr,datum);}

int plato_init(void)
{
     // Load the library
     hLib = LoadLibrary("inpout32.dll");
     if (hLib == NULL) {
          fprintf(stderr,"LoadLibrary Failed.\n");
          return -1;
     }
     // get the addresses of the functions */
     inp32fp = (inpfuncPtr) GetProcAddress(hLib, "Inp32");
     if (inp32fp == NULL) {
          fprintf(stderr,"GetProcAddress for Inp32 Failed.\n");
          return -1;
     }
     oup32fp = (oupfuncPtr) GetProcAddress(hLib, "Out32");
     if (oup32fp == NULL) {
          fprintf(stderr,"GetProcAddress for Oup32 Failed.\n");
          return -1;
     }
	 return 0;
}

// Free the inpout32 library when finished
int plato_stop(void)
{
	FreeLibrary(hLib);
	return 0;
}

// Read input from PPORT_BASE address
void plato_read(void)
{
     short x;
     int i = PPORT_BASE;
	 x = Inp32(i);
	 fprintf(stdout, "Port read (%04X)= %04X\n",i,x);
}

// Write data from parameter
// Examples:
//	test_write_datum(0x00); //open both
//	test_write_datum(0x01); //close L
//	test_write_datum(0x02); //close R
//	test_write_datum(0x03); //close both
void plato_write(short datum) {
     short x;
     int i;

     i = PPORT_BASE;

	 if (datum==0)
		x = 0x00;
	 if (datum==1)
		x = 0x01;
	 if (datum==2)
		x = 0x02;
	 if (datum==3)
		x = 0x03;

     //  Write the data register */
     Out32(i,x);
     fprintf(stdout, "Port write to 0x%X, datum=0x%2X\n" ,i ,x);

     // And read back to verify  */
     x = Inp32(i);
     fprintf(stdout, "Port read (%04X)= %04X\n",i,x);
}
#define PLATO_LEFT_RIGHT_OPEN 0
#define PLATO_LEFT_CLOSED 1
#define PLATO_RIGHT_CLOSE 2
#define PLATO_LEFT_RIGHT_CLOSED 3
int main()
{
	plato_init();
	plato_write(PLATO_LEFT_RIGHT_CLOSED);

	plato_write(PLATO_LEFT_CLOSED);

	plato_write(PLATO_RIGHT_CLOSE);

	plato_write(PLATO_LEFT_RIGHT_OPEN);

	plato_stop();
	return 0;
}
#endif

#include "PlatoGlasses.h"
int main()
{
	plato_init();
	plato_write(PLATO_LEFT_RIGHT_OPEN);
	return 0;
}