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
#include <fstream>
#include <string>
#include <cstring>
#include <limits>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <stdexcept>

#include "ndtypes.h"
#include "ndhost.h"
#include "ndopto.h"
#include "Optotrak.h"

using namespace std;
using namespace Eigen;

/**
*       \ingroup Optotrak
*       Constructor, fill the internal and externalTransformation to identity, recordingTime limit to huge value and frame number to zero.
**/
Optotrak::Optotrak()
{  p3dData=NULL;
   isRecording=false;
   hasTranslation=false;
   recordingTime= (unsigned int) 1E6 ;
   saveLog=false;
   translation.setZero();
}

/**
*       \ingroup Optotrak
*       Destructor, clears all the memory, deactivates the markers and stops the communication with SCU.
**/
Optotrak::~Optotrak()
{  if (p3dData!=NULL)
   {  free(p3dData);
      p3dData=NULL;
   }
}

/**
*       \ingroup Optotrak
*       Initialize the Optotrak.
*       First read from file the optotrakCameraParametersFile camera alignment file, the dataBufferFile and the logFile.
*       Init produce errors if the initialization is not done and prints to standard error.
*       \param  numpoints The number of markers to collect. Internal notation is \f$ [0,\ldots, n) \f$ while Points range from \f$ [1, N] \f$
*       \param optotrakCameraParametersFile The filename of your alignment file. Must be a correct extension. If NULL is provided the default cameraFiles/Aligned20101102 is loaded.
*       \param dataBufferFile Read Optotrak API manual for further informations
*       \param logFileName The name of the log file you want to save on.
*
**/
int Optotrak::init(int numpoints, char *optotrakCameraParametersFile, char *dataBufferFile, char *logFileName)
{  // for now numpoints is ignored, NUM_MARKERS is used instead.
   // Here we allocate memory for the markers used in the experiment
   try
   {  points.reserve(NUM_MARKERS+1);
      points.resize(NUM_MARKERS+1);

      oldpoints.reserve(NUM_MARKERS+1);
      oldpoints.resize(NUM_MARKERS+1);

      oldoldpoints.reserve(NUM_MARKERS+1);
      oldoldpoints.resize(NUM_MARKERS+1);

      velocities.reserve(NUM_MARKERS+1);
      velocities.resize(NUM_MARKERS+1);

      pointsVel.reserve(NUM_MARKERS+1);
      pointsVel.resize(NUM_MARKERS+1);
   }
   catch (std::bad_alloc const&)
   {  cerr << "Memory allocation fail during Optotrak init!" << endl;
   }

   for (vector<Vector3d>::iterator iter = points.begin(); iter!=points.end(); ++iter)
   {  iter->setZero();
   }

   for (vector<Vector3d>::iterator iter = velocities.begin(); iter!=velocities.end(); ++iter)
   {  iter->setZero();
   }

   /// XXX controllare che possa allocare memoria la malloc altrimenti uscire!!!
   try
   {  p3dData = (Position3d*)malloc( NUM_MARKERS * (sizeof( Position3d )) );
      if ( p3dData==NULL )
         throw std::bad_alloc();
   }
   catch ( std::bad_alloc const&)
   {  cerr << "Memory allocation fail during Optotrak init!" << endl;
   }
   // Now initialize the optotrak
   cerr << "1) Transputer load and initialize system..." ;
   if( TransputerLoadSystem( "system" ) != OPTO_NO_ERROR_CODE  || TransputerInitializeSystem( OPTO_LOG_ERRORS_FLAG) )
   {  cerr << "ERROR! Cannot initialize TransputerLoadSystem TransputerInitializeSystem " ;

      return ERRORLOADTRANSPUTER;
   }
   cerr << "OK!" << endl;

   if ( OptotrakSetProcessingFlags( OPTO_LIB_POLL_REAL_DATA | OPTO_CONVERT_ON_HOST | OPTO_RIGID_ON_HOST ) )
   {  cerr << "Can't process on host! " << endl;
      return ERRORSETUPCOLLECTION;
   }

   // Then load the alignment file
   cerr << "2) Transputer load camera file..." ;
   if ( optotrakCameraParametersFile==NULL)
   {  char optotrakCameraParametersFile2[] = "cameraFiles/Aligned20110823";

      if( OptotrakLoadCameraParameters( optotrakCameraParametersFile2 ) != OPTO_NO_ERROR_CODE )
      {  cerr << "ERROR! Please select a correct filename for alignment file" << endl;
         cin.ignore(1E6,'\n');
         return ERRORLOADCAMERA;
      }
      cerr << "File \"cameraFiles/Aligned20110823\"" << endl;
   }
   else
   {  string filename = optotrakCameraParametersFile;
      if( OptotrakLoadCameraParameters( optotrakCameraParametersFile ) != OPTO_NO_ERROR_CODE )
      {  cerr << "ERROR! An error has occurred during loading camera parameter file, please select a correct filename " << endl;
         return ERRORLOADCAMERA;
      }
      cerr <<  filename << "\" loaded successfully" << endl;
   }
   /* Set up a collection for the Optotrak.
   Meaning of OptotrakSetupCollection function
    OptotrakSetupCollection(
               NUM_MARKERS,        // Number of markers in the collection.
               FRAME_RATE,         // Frequency to collect data frames at.
               MARKER_FREQ,      // Marker frequency for marker maximum on-time.
               30,                 // Dynamic or Static Threshold value to use.
               160,                // Minimum gain code amplification to use.
               1,                  // Stream mode for the data buffers.
               DUTY_CYCLE,         // Marker Duty Cycle to use.
               VOLTAGE,         // Voltage to use when turning on markers.
               COLLECTION_TIME,    // Number of seconds of data to collect.
               (float)0.0,         /// Number of seconds to pre-trigger data by.
               OPTOTRAK_NO_FIRE_MARKERS_FLAG  )
   */
   cerr << "3) Optotrak setup collection..." ;
   int setupCollection = OptotrakSetupCollection( NUM_MARKERS, FRAME_RATE, MARKER_FREQ, DYNAMIC_THRESHOLD, MINIMUM_GAIN,
                         STREAM_MODE,  DUTY_CYCLE, VOLTAGE, COLLECTION_TIME, PRETRIGGER , OPTOTRAK_NO_FIRE_MARKERS_FLAG);

   if (setupCollection != OPTO_NO_ERROR_CODE )
   {  cerr << "ERROR! An error has occurred during Optotrak Setup System" << endl;
      return ERRORSETUPCOLLECTION;
   }
   cerr << "OK!" << endl;
   // Wait 2 seconds to let the camera adjust.
   optoTimer.sleep(2000);
   // Activate the markers.
   cerr << "4) Optotrak Activate Marker..." ;
   if( OptotrakActivateMarkers() )
   {  cerr << "ERROR!" << endl;
      return ERRORACTIVATEMARKER;
   }
   cerr << "OK! " << endl;

   optoTimer.sleep(1000);
   // Initialize a file for spooling of the Optotrak 3D data.
   cerr << "5) Optotrak Data Buffer initialize..." ;
   char dataBufferFile2[] = "C#001.S07";
   if( DataBufferInitializeFile( OPTOTRAK, dataBufferFile2 ) )
   {  cerr <<  "ERROR! " << endl;
      return ERRORDATABUFFERINITIALIZE;
   }

   cerr << "OK!" << endl;

   if ( logFileName != NULL )
   {  logFile.open(logFileName);
      saveLog=true;
   }
   else
      saveLog=false;

   optoTimer.start();

   if (saveLog)
      logFile << "# Frame\tdeltaTime\t(px,py,pz)" << endl;

   isRecording=true;

   return OPTOTRAKINITSUCCESSFULL;
}

/**
*       \ingroup Optotrak
*       Set the external user-defined transformation, in this case the transformation is only a translation,
*       that is applied to every point in the collection.
*       Remember!! Once this method is called all the points obtained by the getAllPoints() have this transformation applied!
*
**/
void Optotrak::setTranslation(const Vector3d &_translation)
{  hasTranslation=true;
   translation = _translation;
}

/**
*       \ingroup Optotrak
*       Update the points in the collection: read the markers coordinates and copy them in the vector<Vector3d> points.
*       Faster than updatePointsAndTransformation() method.
*
**/
void Optotrak::updatePoints()
{  beginTime = optoTimer.getElapsedTimeInMilliSec();
   try
   {  if ( DataGetNext3D( &uFrameNumber, &uElements, &uFlags, p3dData) )
         throw std::runtime_error("Cannot collect data at this frame!");
   }
   catch ( std::exception const& e )
   {  cerr << e.what() << endl;
      exit(0);
   }

   for ( int i=1; i<=NUM_MARKERS; i++)
   {  for ( int j=0; j<3; j++)
         oldoldpoints.at(i)[j] = oldpoints.at(i)[j];
   }

   for ( int i=1; i<=NUM_MARKERS; i++)
   {  for ( int j=0; j<3; j++)
         oldpoints.at(i)[j] = points.at(i)[j];
   }

   for ( int i=1; i<=NUM_MARKERS; i++)
   {  points.at(i).x() = p3dData[i-1].x;
      points.at(i).y() = p3dData[i-1].y;
      points.at(i).z() = p3dData[i-1].z;
   }

   endTime= optoTimer.getElapsedTimeInMilliSec()-beginTime;
   // Here we implement the velocity computation by using a backward difference schema of 2nd order
   // x'(t) = (3*x(t) - 4*(x(t-1)) + x(t-2) )/ ( 2*DeltaT )
   // velocities are measured in mm/s
   for ( int i=1; i<=NUM_MARKERS; i++)
   {  if ( (points.at(i).squaredNorm() + oldpoints.at(i).squaredNorm() + oldoldpoints.at(i).squaredNorm()) < 1E30 )
      {  for ( int j=0; j<3; j++)
            //velocities.at(i)[j]=(points.at(i)[j] - oldpoints.at(i)[j]-translation[j] )/( endTime/1000.0 );
            velocities.at(i)[j] = (3*points.at(i)[j] - 4*(oldpoints.at(i)[j] ) + oldoldpoints.at(i)[j] )/(2*endTime/1000.0 );
      }
      else
         velocities.at(i).setZero();
   }

   if (saveLog )
   {  // print optotrak frame Number and time in microseconds
      logFile << uFrameNumber << " " << endTime << " " ;
      for (vector<unsigned int>::iterator iter = savedMarkers.begin(); iter != savedMarkers.end() ; ++iter)
         logFile << points[*iter].transpose() << " ";
      logFile << endl;     //end of frame
   }
}

/**
*       \ingroup Optotrak
*       Start the recording, if putted in background with threads
**/
void Optotrak::startRecording()
{  while ( isRecording )
   {  updatePoints();
   }
}

/**
*       \ingroup Optotrak
*       Stop the background recording.
**/
void Optotrak::stopRecording()
{  isRecording=false;
}

/**
*       \ingroup Optotrak
*       Set the recordingTime, otherwise the recording time is infinity.
*       \param seconds The seconds you want to record
**/
void Optotrak::setRecordingTime(unsigned int seconds)
{  recordingTime = seconds;
}

/**
*       \ingroup Optotrak
*       Return a reference to the points recorded at the latest frame.
**/
vector<Vector3d> Optotrak::getAllPoints()
{  vector<Vector3d> pointret = points;

   // If there is an external translation, compute the translated points transparently
   if (hasTranslation)
   {  for ( int i=1; i<=NUM_MARKERS; i++)
      {  pointret.at(i) -= translation;
      }
   }

   return pointret;
}

/**
*       \ingroup Optotrak
*       Return a reference to the points recorded at the latest frame.
**/
vector<Vector3d> &Optotrak::getAllVelocities()
{  return velocities;
}


/**
*       \ingroup Optotrak
*       Stop the collection, deactivate the markers, turn off the communication with SCU
**/
void Optotrak::stopCollection()
{  isRecording=false;
   if( OptotrakStopCollection( ) != OPTO_NO_ERROR_CODE )
   {  cerr << "Stop data collection" << endl;
   }
   logFile.close();
   OptotrakDeActivateMarkers( );
   TransputerShutdownSystem( );
}

/**
*       \ingroup Optotrak
*       Print the points to output stream
*       \param os stream
**/
void Optotrak::print(ostream &os )
{  for(vector<Vector3d>::iterator iPoint = points.begin(); iPoint!=points.end(); ++iPoint )
   {  os << *iPoint << endl;
   }
   os << endl;
}
/*
 * \ingroup Optotrak
 * Set the markers ID to save in the log file.
 * \param saved is a vector of integers representing the ID of the markers
*/
void Optotrak::setMarkersToSave(const vector<unsigned int> &saved)
{  savedMarkers = saved;
}

int Optotrak::getInternalFrame()
{  return uFrameNumber;
}

// Here the initializations of float variable must be outside
// the header for portability issues
const float Optotrak::FRAME_RATE = 120.0f;   // Should be 4600/(NUM_MARKERS+2)
const float Optotrak::MARKER_FREQ = 3500.0f;
const float Optotrak::DUTY_CYCLE = 0.4f;
const float Optotrak::VOLTAGE=7.0f;
const float Optotrak::COLLECTION_TIME = 6000.0f;
const float Optotrak::PRETRIGGER= 0.0f;

// FASTER CONFIGURATION
/*
const float Optotrak::FRAME_RATE = 160.0f;   // maximum can be 4600/(NUM_MARKERS+2)
const float Optotrak::MARKER_FREQ = 3500.0f;
const float Optotrak::DUTY_CYCLE = 0.4f;
const float Optotrak::VOLTAGE=12.0f;
const float Optotrak::COLLECTION_TIME = 600.0f;
const float Optotrak::PRETRIGGER= 0.0f;
*/
