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

#ifndef _OPTOTRAK_2_H_
#define _OPTOTRAK_2_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>

#include "Marker.h"
#include "DaisyFilter.h"

#include "ndtypes.h"
#include "ndhost.h"
#include "ndopto.h"
#include "Timer.h"

#define OPTOTRAK_INIT_SUCCESSFULL 0

#define OPTOTRAK_FRAME_RATE 100.0f
#define OPTOTRAK_MARKER_FREQ 4600
#define OPTOTRAK_DUTY_CYCLE 0.66f
#define OPTOTRAK_VOLTAGE 10.0f

// DUTY_CYCLE=0.66 and VOLTAGE=10.0 corresponds to 72% markers power
// base settings are 39% markers power, duty-cycle to 0.4 and voltage to 7.0 [V]

using namespace std;
using namespace Eigen;
/**
*       \defgroup Optotrak
*        The Optotrak motion capture system offers maximum flexibility for your motion capture applications through its unsurpassed accuracy,
*       high-speed marker frequency, portable design, and virtually wireless marker option.
*       Built with the superior accuracy and reliability of the renowned Optotrak 3020 series, Optotrak Certus is the gold standard among research scientists.
*       This module deal simply with all the Optotrak API given by the producer NDI and encapsulates them into a easy to use C++ class.
*
*       The class Optotrak2 is the newest version of the previous Optotrak class with the velocity internal computation and data real-time filtering.
*       \class Optotrak2
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
*       Typical correct parameters to have a fully-synchronized visualization-data retrieval system are:       
*       \code
*       int numMarker=20;
*       int frameRate=100;
*       int markerFreq=4600;
*       int dutyCycle=0.4;
*       int voltage=7.0;
*       optotrak.init(numMarker,frameRate,markerFreq,dutyCycle,voltage);
*       \endcode
*       Look the specifications for the init method to see the meaning of the variables.
*       Every time you want to update the data, you must call the updateMarkers() method,
*       then to get the vector of Markers with their coordinates, velocity and acceleration (work in progress) use the getAllMarkers() method
*       \code
*       // Try to pick 10 frames of data
*       vector<Marker> markers
*       for (int i=0; i<10; i++)
*       {
*       optotrak.updateMarkers();
*       // This fills the vector<Vector3d> tmpPoints with all the markers position.
*       markers = optotrak.getAllMarkers();
*       }
*       \endcode
*       Some utility methods are provided in order to simplify the usage of Optotrak API.
*       If you are working in a reference frame translated
*       with respect to the internal Optotrak's reference frame, you simply should call the
*       \code setTranslation(const Vector3d &translation) \endcode method.
*       After this call every marker position is translated in you new reference frame.
*
*       When the program is finished or when you meet stop conditions, remember to calls Optotrak::stopCollection() in order to deactivate the markers
*       and stop the communications between SCU and PC. Otherwise the destructor will do it for you.
**/

class Optotrak2
{
public:
   Optotrak2();
   ~Optotrak2();

// Public methods
   int init(char *optotrakCameraParametersFile, int numMarker=20,
            float frameRate=OPTOTRAK_FRAME_RATE,
            float markerFreq=OPTOTRAK_MARKER_FREQ,
            float dutyCycle=OPTOTRAK_DUTY_CYCLE,
            float voltage=OPTOTRAK_VOLTAGE);
   void setTranslation(const Vector3d&);

   void updateMarkers(double deltaTInMilliSec=10);	//by default 10 milliseconds

   std::vector<Marker> &getAllMarkers();
   std::vector<Vector3d>& getAllVelocities();

   void stopCollection();
   int getInternalFrame();
private:
   // Optotrak variables, first integer variables that can be declared inside the header
   int NUM_MARKERS;    // Number of markers in the collection.
   int DELAY;
   int DYNAMIC_THRESHOLD;  // Dynamic or Static Threshold value to use.
   int MINIMUM_GAIN;    // Minimum gain code amplification to use.
   int STREAM_MODE;    // Stream mode for the data buffers.
   //float MARKER_FREQ ;   //Marker frequency for marker maximum on-time.
   //float FRAME_RATE;    //  Frequency to collect data frames at
   //float DUTY_CYCLE;    // Marker Duty Cycle to use.
   //float VOLTAGE;    // Voltage to use when turning on markers.
   float COLLECTION_TIME;    /* Number of seconds of data to collect. */
   float PRETRIGGER;         /* Number of seconds to pre-trigger data by. */

   // Filters to act real-time on data
   vector< vector< DaisyFilter*> > filters;
   // Optotrak API structures
   Position3d *p3dData;
   unsigned int uFlags;
   unsigned int uElements;
   unsigned int uFrameNumber;

   // C++ Optotrak Class structures (defined by the class)
   vector<Vector3d> points[3];
   vector<Vector3d> velocities;
   vector < Marker > markers;
   Vector3d translation;

   // Timing variables
   Timer optoTimer;
   bool optotrakInitializated;

};
#endif


