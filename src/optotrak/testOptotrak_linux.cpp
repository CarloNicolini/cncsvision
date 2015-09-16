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

#include <iostream>
#include <string>
#include "Timer.h"
#include "ndtypes.h"
#include "ndpack.h"
#include "ndopto.h"
#include <cstring>
Timer timer;

using namespace std;

void print_errors()
{
    char szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];
    if( OptotrakGetErrorString( szNDErrorString, MAX_ERROR_STRING_LENGTH + 1 ) == 0 )
    {
        cerr << szNDErrorString << endl;
    }
}

int main(int argc, char*argv[])
{

    //OptotrakSettings            dtSettings;
    char szProperty[32];
    int i;
    int nCurDevice;
    int nCurProperty;
    int nCurFrame;
    int nCurMarker;
    int nMarkersToActivate;
    int nDevices;
    int nDeviceMarkers;
    //ApplicationDeviceInformation *pdtDevices;
    //DeviceHandle *pdtDeviceHandles;
    //DeviceHandleInfo *pdtDeviceHandlesInfo;
    unsigned int uFlags, uElements, uFrameNumber;
    Position3d *p3dData;
    char *pChar;
    char szInput[10];

    cerr << TransputerLoadSystem("system") << endl;
    print_errors();
    sleep(1);
    cerr << TransputerInitializeSystem( OPTO_LOG_VALID_FLAGS | OPTO_LOG_DEBUG_FLAG | OPTO_LOG_CONSOLE_FLAG | OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG ) << endl;
    print_errors();
    cerr << OptotrakSetProcessingFlags( OPTO_LIB_POLL_REAL_DATA | OPTO_CONVERT_ON_HOST | OPTO_RIGID_ON_HOST ) << endl;
    print_errors();
    cerr << OptotrakLoadCameraParameters( "x" ) << endl;
    print_errors();
    cerr << "qui" << endl;
    return 0;
}

