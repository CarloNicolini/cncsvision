// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2010-2014 Carlo Nicolini <carlo.nicolini@iit.it>
//
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

#ifndef _PLATO_GLASSES_H_
#define _PLATO_GLASSES_H_

#include <cstdlib>
#include <direct.h>
#include <cstdio>
#include <conio.h>

#include <windows.h>
#include <MMSystem.h>

#define PARALLEL_PORT_BASE 0x378 // this is the port's signal pin
#define PLATO_LEFT_RIGHT_OPEN 0
#define PLATO_LEFT_CLOSED 1
#define PLATO_RIGHT_CLOSE 2
#define PLATO_LEFT_RIGHT_CLOSED 3

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
static HINSTANCE hLib;// = LoadLibrary("inpout32.dll");

// After successful init, these 2 variables will contain function pointers.*/
static inpfuncPtr inp32fp;
static oupfuncPtr oup32fp;

// Wrapper functions for the function pointers - call these to perform I/O. */
short  Inp32 (short portaddr);
void  Out32 (short portaddr, short datum);

#endif
