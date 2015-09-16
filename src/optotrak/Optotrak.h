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

#ifndef _OPTOTRAK_H
#define _OPTOTRAK_H

#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>


#include "ndtypes.h"
#include "ndhost.h"
#include "ndopto.h"
#include "Timer.h"

#define OPTOTRAKINITSUCCESSFULL 0
#define ERRORLOADTRANSPUTER 1
#define ERRORLOADCAMERA 2
#define ERRORSETUPCOLLECTION 3
#define ERRORACTIVATEMARKER 4
#define ERRORDATABUFFERINITIALIZE 5

using namespace std;
using namespace Eigen;
/**
*       \defgroup Optotrak
*        The Optotrak motion capture system offers maximum flexibility for your motion capture applications through its unsurpassed accuracy,
*       high-speed marker frequency, portable design, and virtually wireless marker option.
*       Built with the superior accuracy and reliability of the renowned Optotrak 3020 series, Optotrak Certus is the gold standard among research scientists.
*       This module deal simply with all the Optotrak API given by the producer NDI and encapsulates them into a easy to use C++ class.
*
*       \class Optotrak
*       \ingroup Optotrak
*       \brief This class is a useful object-oriented wrapper around the calls to Optotrak API.
*
*       A simple example is given in the test.cpp
*       First declare a Optotrak object:
*       \code
*       Optotrak optotrak;
*       \endcode
*       Then initialize it: the method call all the necessary functions to initialize the communication between the SCU and the PC,
*       supposed that the Optotrak Device Drivers are correctly installed.
*       \code
*       int numMarker=10;
*       optotrak.init(numMarker,NULL,NULL,"optotrak.log");
*       \endcode
*       Look the specifications for the init method to see the meaning of the variables.
*       Every time you want to update the data (a wrapper around Position3d using the Vector3d class is used), you must call the updatePoints() method,
*       then to get the vector of Vector3d use the getAllPoints() method
*       \code
*       // This save a frame of points with the maximum possible sampling frequency for the specified number of markers
*       optotrak.updatePoints();
*       // This fills the vector<Vector3d> tmpPoints with all the markers position.
*       vector<Vector3d> tmpPoints = optotrak.getAllPoints();
*       \endcode
*       Some utility methods are provided in order to simplify the usage of Optotrak API.
*       If you are working in a reference frame that is translated
*       with respect to the internal Optotrak's reference frame, you simply should call the
*       \code setTranslation(const Vector3d &translation) \endcode method.
*       After this call every marker position is translated in you new reference frame.
*       Optotrak can also work in background during the execution of your main program, this feature allows you to fire and forget.
*       The Optotrak class continously updates the markers coordinates and save them in a log file with specified name given in the Optotrak::init(...)
*       You can detach this process from the main() flow using the boost libraries. This is simply accomplished with the following lines of code:
*       \code
*       // First set for how many seconds you want to record (if this method isn't called time is infinity)
*       optotrak.setRecordingTime(5);
*       // Then declare a boost::thread that repeats the Optotrak::startRecording in background until the program finish.
*       boost::thread optorec(&Optotrak::startRecording, &optotrak );
*       // Detach this thread from the principal flow of instructions
*       optorec.detach();
*       \endcode
*       Then everytime you want to update your points position, you simply call Optotrak::getAllPoints() and the Optotrak class
*       automagically fills the vector<Vector3d>  with no time or space conflicts (the points are correctly synchronized)
*
*       When the program is finished or when you meet stop conditions, remember to calls Optotrak::stopCollection() in order to deactivate the markers
*       and stop the communications between SCU and PC.
*       In this example we explain a typical usage of the \ref Optotrak class:
*       \code
*        #include "Optotrak.h"
*
*       int main(void)
*       {
*               Optotrak optotrak;
*               optotrak.setTranslation(calibration);
*          if ( optotrak.init(NULL,NULL,"optotrakGL.log") != 0)
**          exit(0);
*       optotrak.setRecordingTime(5);
*       // Then declare a boost::thread that repeats the Optotrak::startRecording in background until the program finish.
*       boost::thread optorec(&Optotrak::startRecording, &optotrak );
*       // Detach this thread from the principal flow of instructions
*       optorec.detach();
*       return 0;
*       \endcode
**/

class Optotrak
{
public:
   Optotrak();
   ~Optotrak();

// Public methods
   int init(int numpoints, char *optotrakCameraParametersFile, char *dataBufferFile, char* logFileName=NULL );
   void setTranslation(const Vector3d&);

   void updatePoints();

   std::vector<Vector3d> getAllPoints();
   std::vector<Vector3d>& getAllVelocities();
   std::vector< pair<Vector3d,Vector3d> >& getAllPointsAndVelocities();

   void stopCollection();
   void print(ostream &);
   void printStats(ostream &);
   void resetFrameNumber()
   {  trialFrameNumber=0;
   };

   void startRecording();
   void stopRecording();
   void setRecordingTime(unsigned int );

   void setMarkersToSave(const vector<unsigned int> & );
   int getInternalFrame();
private:
   // Optotrak API structures
   Position3d *p3dData;
   unsigned int uFlags, uElements, uFrameNumber ;

   // C++ Optotrak Class structures (defined by the class)
   vector<Vector3d> points, oldpoints,oldoldpoints;
   vector<Vector3d> velocities;
   vector< pair<Vector3d,Vector3d> > pointsVel;

   Vector3d translation;
   vector<unsigned int> savedMarkers;
   ofstream logFile;
   Timer optoTimer;
   unsigned int trialFrameNumber;
   double beginTime ,endTime;

   // Thread dependent part
   // mutex necessary to lock the access to vector<Vector3d> points data from other threads

   bool isRecording;
   bool hasTranslation;
   bool saveLog;
   unsigned int recordingTime;
   // Optotrak variables, first integer variables that can be declared inside the header
   static const int NUM_MARKERS = 20;    // Number of markers in the collection.
   static const int DELAY = 1;
   static const int DYNAMIC_THRESHOLD = 30;  // Dynamic or Static Threshold value to use.
   static const int MINIMUM_GAIN = 160;    // Minimum gain code amplification to use.
   static const int STREAM_MODE= 1;    // Stream mode for the data buffers.
   // then floating point variables
   static const float MARKER_FREQ ;   //Marker frequency for marker maximum on-time.
   static const float FRAME_RATE;    //  Frequency to collect data frames at
   static const float DUTY_CYCLE;    // Marker Duty Cycle to use.
   static const float VOLTAGE;    // Voltage to use when turning on markers.
   static const float COLLECTION_TIME;    /* Number of seconds of data to collect. */
   static const float PRETRIGGER;         /* Number of seconds to pre-trigger data by. */
};
#endif
