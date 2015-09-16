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


#ifdef _WIN32
#ifndef NOEYEGAZE

#include "EyeFollower.h"
#include <boost/thread/thread.hpp>
#include <fstream>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
/** \ingroup Communication
Constructor
**/
EyeFollower::EyeFollower() : isBusy(false), stopRequested(false), gazeX(0),gazeY(0),bufferLen(120)
{
}

/**
* \ingroup Communication
* Destructor
**/
EyeFollower::~EyeFollower()
{  this->stopBackgroundCollection();
   cerr << "Closing EyeFollower connection, calling EgExit" << endl;
   EgExit(&stEgControl);
}

/**
* \ingroup Communication
* Main init method, it takes four parameters needed to setup your system and get it ready to run.
* \param calibrate True if you want to perform a calibration before starting the experiment
* \param width The width in pixel of your display active area (check that the value here is the same as the value specified on the server-side machine in the area width)
* \param height The width in pixel of your display active area (check that the value here is the same as the value specified on the server-side machine in the area height)
* \param connection A string: SOCKET makes a ethernet connection between client and server (EG_COMM_TYPE_SOCKET), otherwise the connection type is set as EG_COMM_TYPE_LOCAL
* \param address The address of the server machine to reach. If connection is EG_COMM_TYPE_LOCAL then address is automatically set the loopback interface 127.0.0.1
**/
void EyeFollower::init(bool calibrate, int width, int height, char* connection, char* address )
{  int connectionType=0;
   if ( string(connection) == "SOCKET" )
      connectionType = EG_COMM_TYPE_SOCKET;
   else
      connectionType = EG_COMM_TYPE_LOCAL;

   stEgControl.iNDataSetsInRingBuffer = 120;
   stEgControl.iScreenWidthPix=width;
   stEgControl.iScreenHeightPix=height;
   stEgControl.bEgCameraDisplayActive = FALSE;
   stEgControl.iEyeImagesScreenPos = 1;
   stEgControl.iVisionSelect=0;
   stEgControl.iCommType = connectionType;
   if ( connectionType == EG_COMM_TYPE_SOCKET )
      stEgControl.pszCommName = char2wchar( address );
   else
      stEgControl.pszCommName = char2wchar( "127.0.0.1" );
   cerr << "Initializing stEgControl structure..."  ;
   int errorValue =  EgInit(&stEgControl);
   cerr << "ok" << endl;
   if ( calibrate )
   {  EgCalibrate2(&stEgControl, EG_CALIBRATE_NONDISABILITY_APP);
      cerr << "Calibration done correctly" << endl;
   }
   else
      cerr << "No calibration done, using last Calibration.dat" << endl;
   stEgControl.bTrackingActive = TRUE;
   std::ofstream outputdebugInfo;
   outputdebugInfo.open("efDebug.txt");
   printInfo(outputdebugInfo);

}
/** \ingroup Communication
* Get the current path where the executable is running.
* \return The current executable path
**/
std::string EyeFollower::getExePath()
{  char buffer[MAX_PATH];
   GetModuleFileName( NULL, buffer, MAX_PATH );
   std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
   return std::string( buffer ).substr( 0, pos);
}

/** \ingroup Communication
* Print the Eg internal variables to output stream
**/
void EyeFollower::printInfo(std::ostream &os)
{  os <<"stEgControl.bEgCameraDisplayActive= " << stEgControl.bEgCameraDisplayActive << endl;
   os <<"stEgControl.bTrackingActive= " << stEgControl.bTrackingActive << endl;
   os <<"stEgControl.fHorzPixPerMm= " << stEgControl.fHorzPixPerMm << endl;
   os <<"stEgControl.fVertPixPerMm= " << stEgControl.fVertPixPerMm << endl;
   os <<"stEgControl.hEyegaze= " << stEgControl.hEyegaze << endl;
   os <<"stEgControl.iCommType= " << stEgControl.iCommType << endl;
   os <<"stEgControl.iEyeImagesScreenPos= " << stEgControl.iEyeImagesScreenPos << endl;
   os <<"stEgControl.iNBufferOverflow= " << stEgControl.iNBufferOverflow << endl;
   os <<"stEgControl.iNDataSetsInRingBuffer= " << stEgControl.iNDataSetsInRingBuffer << endl;
   os <<"stEgControl.iNPointsAvailable= " << stEgControl.iNPointsAvailable << endl;
   os <<"stEgControl.iSamplePerSec= " << stEgControl.iSamplePerSec << endl;
   os <<"stEgControl.iScreenHeightPix= " << stEgControl.iScreenHeightPix << endl;
   os <<"stEgControl.iScreenWidthPix= " << stEgControl.iScreenWidthPix << endl;
   os <<"stEgControl.pstEgData= " << stEgControl.pstEgData << endl;
   os <<"stEgControl.pvEgVideoBufferAddress= " << stEgControl.pvEgVideoBufferAddress << endl;
}

/** \ingroup Communication
* Convert a string to wstring (Windows only)
**/
std::wstring EyeFollower::string2wstring(const std::string& s)
{  int len;
   int slength = (int)s.length() + 1;
   len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
   wchar_t* buf = new wchar_t[len];
   MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
   std::wstring r(buf);
   delete[] buf;
   return r;
}

/** \ingroup Communication
* Convert a char to wchar
**/
wchar_t* EyeFollower::char2wchar(char* orig)
{  // Convert to a wchar_t*
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

/** \ingroup Communication
* Send the Eyefollower the EgGetData command and update the gaze coordinates
**/
bool EyeFollower::querySystem()
{  /*
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

//#define VERGENCE_TEST
#if defined VERGENCE_TEST

   int bVergenceFound;
//      Get the vergence data only if the gaze data is ahead of the vergence data in time.
   if (stEgControl.pstEgData->ulCameraFieldCount > stGazeVergence.ulCameraFieldCount)
   {
//          Get vergence data and make sure it's in synch with the gaze data.
      do
      {  bVergenceFound = bGetVergenceAnd3DGazePoint(&stGazeVergence);
      }
      while (stGazeVergence.ulCameraFieldCount < stEgControl.pstEgData->ulCameraFieldCount);
   }
#endif
   int iVis = EgGetData(&stEgControl);
   return iVis;
}

/** \ingroup Communication
* Get the gaze position in pixel
**/
Eigen::Vector2i EyeFollower::getGazePos()
{  boost::mutex::scoped_lock l(this->eyeFollowerMutex);
   return Eigen::Vector2i(stEgControl.pstEgData->iIGaze, stEgControl.pstEgData->iJGaze);
}

/** \ingroup Communication
* Check if the Eyefollower found the gaze
* \return True if the Eyefollower found the gaze
**/
bool EyeFollower::isVisible()
{  return (bool)stEgControl.pstEgData->bGazeVectorFound;
}

/** \ingroup Communication
* Loop the data getting function
**/
void EyeFollower::dataCollectionLoop()
{  while ( !this->stopRequested )
   {  boost::mutex::scoped_lock l(this->eyeFollowerMutex);
      this->querySystem();
   }
   boost::mutex::scoped_lock l(this->eyeFollowerMutex);
   this->isBusy= false;
   this->stopRequested=false;
}

/** \ingroup Communication
* Start to collect data in background in a separate thread. Loops while EyeFollower::stopBackgroundCollection() is called.
**/
void EyeFollower::startBackgroundCollection()
{  if ( !this->isBusy )
   {  cerr << "Starting background data acquisition...";
      this->isBusy = true;
      this->stopRequested= false;
      assert(!this->dataLoopThread);
      this->dataLoopThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&EyeFollower::dataCollectionLoop, this)));
      cerr << "ok" << endl;
   }
}

/** \ingroup Communication
* Send the stop to background data collection. LEDs on the Eyefollower remains active but no data are transmitted.
**/
void EyeFollower::stopBackgroundCollection()
{  if (this->isBusy == true)
   {  stEgControl.bTrackingActive = FALSE;
      assert(this->dataLoopThread);
      cerr << "Blocking data acquisition..." ;
      this->stopRequested = true;
      this->dataLoopThread->join();
      this->isBusy = false;
      cerr << "ok" << endl;
   }
}
#endif // NOEYEGAZE
#endif // _WIN32
