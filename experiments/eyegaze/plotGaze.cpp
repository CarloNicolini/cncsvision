// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2011 Carlo Nicolini <carlo.nicolini@iit.it>
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

#include <windows.h>
#include <process.h>
#include <string>
#include <iostream>
#include <fstream>
#include <EgWin.h>
#include <EgErrorMessage.h>
#include "Timer.h"
#include <boost\lexical_cast.hpp>

using namespace std;
// Declaration of EyeGaze data structures
// The eyetracking application must define and fill this Eyegaze control structure.
static struct _stEgControl stEgControl;

#define EG_BUFFER_LEN 60

void printInfo(const _stEgControl &stEgControl)
{
	ofstream outputfile;
	outputfile.open("info.txt");
	outputfile << "stEgControl.bEgCameraDisplayActive= " << stEgControl.bEgCameraDisplayActive << endl;
	outputfile << "stEgControl.bTrackingActive= " << stEgControl.bTrackingActive << endl;
	outputfile << "stEgControl.fHorzPixPerMm= " << stEgControl.fHorzPixPerMm << endl;
	outputfile << "stEgControl.fVertPixPerMm= " << stEgControl.fVertPixPerMm << endl;
	outputfile << "stEgControl.hEyegaze= " << stEgControl.hEyegaze << endl;
	outputfile << "stEgControl.iCommType= " << stEgControl.iCommType << endl;
	outputfile << "stEgControl.iEyeImagesScreenPos= " << stEgControl.iEyeImagesScreenPos << endl;
	outputfile << "stEgControl.iNBufferOverflow= " << stEgControl.iNBufferOverflow << endl;
	outputfile << "stEgControl.iNDataSetsInRingBuffer= " << stEgControl.iNDataSetsInRingBuffer << endl;
	outputfile << "stEgControl.iNPointsAvailable= " << stEgControl.iNPointsAvailable << endl;
	outputfile << "stEgControl.iSamplePerSec= " << stEgControl.iSamplePerSec << endl;
	outputfile << "stEgControl.iScreenHeightPix= " << stEgControl.iScreenHeightPix << endl;
	outputfile << "stEgControl.iScreenWidthPix= " << stEgControl.iScreenWidthPix << endl;
	outputfile << "stEgControl.pstEgData= " << stEgControl.pstEgData << endl;
	outputfile << "stEgControl.pvEgVideoBufferAddress= " << stEgControl.pvEgVideoBufferAddress << endl;
}

std::wstring s2ws(const std::string& s)
{
 int len;
 int slength = (int)s.length() + 1;
 len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
 wchar_t* buf = new wchar_t[len];
 MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
 std::wstring r(buf);
 delete[] buf;
 return r;
}

wchar_t* char2wchar(char* orig)
{
    // Convert to a wchar_t*
    size_t origsize = strlen(orig) + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
    //wchar_t* wcstring = new wchar_t[newsize];
    wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, orig, _TRUNCATE);
    wcscat_s(wcstring, L" (wchar_t *)");
    //wcout << wcstring << endl;

	wchar_t *retString = new wchar_t[newsize];
	for (int i=0; i<newsize; i++)
		retString[i]=wcstring[i];

	return retString;
}


std::string getExePath()
{
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    return string( buffer ).substr( 0, pos);
}


int main(int argc, char *argv[])
{
	// Very important, this must be set so to link to the eyegaze drivers!
	cout << "Current directory set succeed? " << SetCurrentDirectory("C:\\Eyegaze\\") << endl;
	cout << "Current dir is " << getExePath() << endl;
	//spawnl(P_WAIT, "Calibrate.exe", "Calibrate.exe", NULL);
	
	// Tell the Eyegaze the length of Eyegaze data ring buffer
	stEgControl.iNDataSetsInRingBuffer = EG_BUFFER_LEN;
	// Tell Eyegaze not to begin image processing yet so no past gazepoint data samples will have accumulated in the ring buffer when the tracking loop begins
	//stEgControl.bTrackingActive=FALSE;

	// Tell Eyegaze the dimension in pixels of the client window
	int  iScreenWidth  = 1024;
    int iScreenHeight = 768;

	stEgControl.iScreenWidthPix=iScreenWidth;
	stEgControl.iScreenHeightPix=iScreenHeight;

	stEgControl.bEgCameraDisplayActive = FALSE;
	/* Tell Eyegaze to display the full    */ 
	/*   640x480 camera image in a separate    */ 
	/*   window.                               */ 
	stEgControl.iEyeImagesScreenPos = 1;
	/* Tell Eyegaze that the location for the  
	eye image display is the upper right   
	corner                                
		1 -- upper right corner               
		2 -- upper left corner
	*/
	stEgControl.iVisionSelect=0;  // Set this reserved variable to 0 

	// The communications type may be set to one of three values. 
	//stEgControl.iCommType = EG_COMM_TYPE_LOCAL;   // Eyegaze Single Computer Configuration 
	// stEgControl.iCommType = EG_COMM_TYPE_SERIAL;  // Eyegaze Double Computer Configuration 
	stEgControl.iCommType = EG_COMM_TYPE_SOCKET;  // Eyegaze Double Computer Configuration ethernet connection

	// If the comm type is socket or serial, set one of the following:           
	// stEgControl.pszCommName = "COM1";        // Eyegaze comm port for EG_COMM_TYPE_SERIAL 
	
	stEgControl.pszCommName = char2wchar( "192.168.231.179" );   // Eyegaze server IP address for EG_COMM_TYPE_SOCKET 

	// Create the Eyegaze image processing thread
	cerr << "Initializing stEgControl structure..."  ;
	int errorValue =  EgInit(&stEgControl);
	spawnl(P_WAIT,"calibrate.exe","CALIBRATE.EXE","runtime",NULL);
	//EgCalibrate2(&stEgControl, EG_CALIBRATE_NONDISABILITY_APP);
	/*
	Graphics Support: If the application program implements the Eyegaze calibration
	procedure by calling the EgCalibrate() function rather than by spawning the Calibrate.exe
	program, the application program must use the LCT graphics support code discussed
	in Appendix I. If the program uses other graphics support code, it must spawn the
	Calibrate.exe program rather than call the EgCalibrate() function. See "Graphics Support
	for Calibration", Section 4.17.
	*/
	//cerr << "Calibrate2 ..."  ;

	//cerr << "done..."  ;
	Timer timer;
	timer.start();

	/*
	This code keeps the loop synchronized with the real-time Eyegaze      
	image processing, but insures that all gazepoint data samples are      
	processed, even if the loop gets up to two seconds behind the         
	real-time Eyegaze image processing.    
	*/
	stEgControl.bTrackingActive = TRUE;
	printInfo(stEgControl);
	cerr << "Collecting data" << endl;
	while ( timer.getElapsedTimeInSec() < 5 )
	{
		/*
		Get the next gazepoint sample.  If an unprocessed Eyegaze data sample 
		is still available, EgGetData() returns immediately, allowing the     
		application to catch up with the Eyegaze image processing.  If the    
		next unprocessed sample has not yet arrived, EgGetData blocks until   
		data is available and then returns.  This call effectively puts the    
		application to sleep until new Eyegaze data is available to be        
		processed.                                                             
		The image processing software, running independently of this          
		application, produces a new eyegaze data sample every 16.67 milli-    
		seconds. If an unprocessed Eyegaze data sample is still available     
		for processing, EgGetData() returns immediately, allowing the         
		application to catch up with the Eyegaze image processing.  If the    
		next unprocessd sample has not yet arrived, EgGetData blocks until    
		data is available and then returns.  This call effectively puts the  
		application to sleep until new Eyegaze data is available to be        
		processed.   
		*/
		EgGetData(&stEgControl); 
		/*
		Process the next gazepoint data sample – contained in pstEgData.       
		Convert the gazepoint from full-screen to client window coordinates.  
		*/
		Sleep(100);
		int xGaze =  stEgControl.pstEgData->iIGaze;
		int yGaze = stEgControl.pstEgData->iJGaze;
		int gazeFound = stEgControl.pstEgData->bGazeVectorFound;
		cerr << xGaze << " " << yGaze << " " << gazeFound << endl;
	}
	cerr << "Deactivating system" << endl;
	EgExit(&stEgControl);

	return 0;
}