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

#ifndef _EYEFOLLOWER_H_
#define _EYEFOLLOWER_H_

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#include <EgWin.h>
#endif
#include <Eigen/Core>
#include <iostream>
#include <string>
#include <boost\thread\thread.hpp>
#include <boost\lexical_cast.hpp>

/*
\class EyeFollower
\ingroup Communication
* \brief EyeFollower wrapper class for the Eg functions.
* This class help the user to work with an Eyefollower once it is correctly configured.
* In order for this class to work you have to specify the following (Visual Studio 2010 instructions)
* 1) Include the C:\Eyegaze directory (or another equivalent directory) from "Project Properties -> VC++ Directories -> Include directories"
* 2) Add the EgClient.lib import library in the linker additional dependencies.
* 3) Copy the C:\Eyegaze\EgClient.dll and C:\Eyegaze\egfileio.dll in your executable directory
*
* Then the class initialization works as follows:
*
* \code
* EyeFollower eyefollower;
* bool calibrate=true;
* int width=640, height=480;
* char *connection = "SOCKET";
* char *address = "192.168.231.179";
* eyefollower.init( calibrate, width, height, connection, address );
* \endcode
*
* At this point you have to decide if you want to do a the data grabbing in a separate thread (asynchronous)
* or using a synchronous data updating and grabbing.
* For the ASYNCHRONOUS data grabbing use the following approach:
* \code
* eyefollower.startBackgroundCollection();
* // INSIDE YOUR MAIN PROGRAM LOOP
* {
*     int ex = eyefollower.getGazePos().x();
*     int ey = eyefollower.getGazePos().y();
* }
* \endcode
* while for the synchronous calls you have to query the system inside your main loop and then get data
* \code
*
* // INSIDE YOUR MAIN PROGRAM LOOP
* while ( is your main loop running )
* {
*     eyefollower.querySystem();
*     int ex = eyefollower.getGazePos().x();
*     int ey = eyefollower.getGazePos().y();
* }
* \endcode
* The errors encountered in initializing the system are printed to a debug file in the same directory of the executable named "efDebug.txt" .
*/

class EyeFollower
{
public:
	EyeFollower();
	~EyeFollower();
	void init(bool calibrate, int width,int height, char* connection, char* address);
	
	void startBackgroundCollection();
	void stopBackgroundCollection();
	bool querySystem();
	bool isVisible();
	Eigen::Vector2i getGazePos();
	double getVergence();
	
	void printInfo(std::ostream &os);
	
private:
	// mutex - locks following variables to current thread
	boost::mutex eyeFollowerMutex;
	bool isBusy, stopRequested;
	void dataCollectionLoop();
	std::string getExePath();

	std::wstring string2wstring(const std::string& s);
	wchar_t* char2wchar(char* orig);
	int gazeX,gazeY, bufferLen;

	//boost::thread *dataLoopThread;
	// thread pointer
	boost::shared_ptr<boost::thread> dataLoopThread;
	#ifdef _WIN32
	struct _stEgControl stEgControl;
	#endif
};

//#endif

#endif
