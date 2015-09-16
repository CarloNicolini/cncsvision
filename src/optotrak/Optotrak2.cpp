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
#include <stdexcept>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>

#include "ndtypes.h"
#include "ndhost.h"
#include "ndopto.h"
#include "Marker.h"
#include "Optotrak2.h"

using namespace std;
using namespace Eigen;

void print_errors()
{
    char szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];
    if( OptotrakGetErrorString( szNDErrorString, MAX_ERROR_STRING_LENGTH + 1 ) == 0 )
    {
        cerr << szNDErrorString << endl;
    }
}

/**
*       \ingroup Optotrak
*       Constructor, fill the internal and externalTransformation to identity, recordingTime limit to huge value and frame number to zero.
**/
Optotrak2::Optotrak2()
{
    p3dData=NULL;
    translation.setZero();
    COLLECTION_TIME = 6000;
    PRETRIGGER= 0.0f;
    NUM_MARKERS = 20;    // Number of markers in the collection.
    DELAY = 1;
    DYNAMIC_THRESHOLD = 30;  // Dynamic or Static Threshold value to use.
    MINIMUM_GAIN = 180;    // Minimum gain code amplification to use.
    STREAM_MODE= 1;
}

/**
*       \ingroup Optotrak
*       Destructor, clears all the memory, deactivates the markers and stops the communication with SCU.
**/
Optotrak2::~Optotrak2()
{
    if ( this->optotrakInitializated )
    {
        this->stopCollection();
        if (p3dData!=NULL)
        {  free(p3dData);
            p3dData=NULL;
        }
        for (int i=0; i<=NUM_MARKERS; i++)
        {  delete filters[i][0];
            delete filters[i][1];
            delete filters[i][2];
            delete filters[i][3];
            delete filters[i][4];
            delete filters[i][5];
        }
        cerr << "[Optotrak] Destructor" << endl;
    }
}

/**
*       \ingroup Optotrak
*       Initialize the Optotrak.
*       First read from file the optotrakCameraParametersFile camera alignment file.
*       Init produce errors if the initialization is not done and prints to standard error.
*       \param optotrakCameraParametersFile The filename of your alignment file. Must be a correct extension. If NULL is provided the default standard.cam is loaded.
*       \param  numMarker The number of markers to collect. Internal notation is \f$ [0,\ldots, n) \f$ while Points range from \f$ [1, N] \f$
*       \param frameRate Frequency to collect data frames at.  (default is 100 Hz)
*       \param markerFreq  Marker frequency for marker maximum on-time.       (default is 4600)
*       \param dutyCycle The duty cycle of the markersoptotrakCameraParametersFile, i.e. the time they are on (0.4 is default)
*       \param voltage Voltage to use when turning on markers (7 volt default).
*
*
**/
int Optotrak2::init(char *optotrakCameraParametersFile, int numMarker, float frameRate, float markerFreq, float dutyCycle, float voltage)
{
    optotrakInitializated=false;
    NUM_MARKERS=numMarker;

    // for now numpoints is ignored, NUM_MARKERS is used instead.
    // Here we allocate memory for the markers used in the experiment
    // for now numpoints is ignored, NUM_MARKERS is used instead.
    // Here we allocate memory for the markers used in the experiment
    try
    {
        markers.resize(NUM_MARKERS+1);
        for (int i=0; i<3; i++)
        {  points[i].resize(NUM_MARKERS+1);
            for (vector<Vector3d>::iterator iter = points[i].begin(); iter!=points[i].end(); ++iter)
            {  iter->setZero();
            }
        }
        velocities.resize(NUM_MARKERS+1);
        for (vector<Vector3d>::iterator iter = velocities.begin(); iter!=velocities.end(); ++iter)
            iter->setZero();

        filters.resize(NUM_MARKERS+1);
        for (int i=0; i<=NUM_MARKERS; i++)
        {  filters[i].resize(6);
            for ( int j=0; j<3; j++)
                filters[i][j] = DaisyFilter::SinglePoleIIRFilter(0.8); //filter for positions
            for ( int j=3; j<6; j++)
                filters[i][j] = DaisyFilter::SinglePoleIIRFilter(0.3); // filter for velocities (smoother)
        }
    }
    catch (std::exception const &e)
    {  cerr << "[Optotrak] Memory allocation fail during Optotrak init: " << e.what() << endl;
    }

    /// XXX controllare che possa allocare memoria la malloc altrimenti uscire!!!
    p3dData = (Position3d*)calloc( NUM_MARKERS , (sizeof( Position3d )) );
    if ( p3dData==NULL )
        throw std::runtime_error("[Optotrak] Memory allocation fail during Optotrak init! ");

#if SIMULATION
    return OPTOTRAK_INIT_SUCCESSFULL;
#else
    // Now initialize the optotrak
    cerr << "[Optotrak] Transputer load and initialize system..." << endl;
    if( TransputerLoadSystem( "system" ) != OPTO_NO_ERROR_CODE  )
    {
        print_errors();
#ifdef __linux__
        cerr << "Please check that environment variable ND_DIR is set to /usr/NDIoapi/ndigital" << endl;
#endif
        throw std::runtime_error("[Optotrak] Bad initialization");
    }
    if ( TransputerInitializeSystem(OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG) != OPTO_NO_ERROR_CODE )
    {
        print_errors();
        throw std::runtime_error("[Optotrak] Bad initialization");
    }
    this->optotrakInitializated=true;
    if ( OptotrakSetProcessingFlags( OPTO_LIB_POLL_REAL_DATA | OPTO_CONVERT_ON_HOST | OPTO_RIGID_ON_HOST ) )
    {
        print_errors();
        throw std::runtime_error("[Optotrak] Can't process on host!");
    }

    // Then load the alignment file
    cerr << "[Optotrak] Transputer load camera file..." << endl;
#ifdef __linux__
    std::string camera_file(optotrakCameraParametersFile);
    for (int i=0; i<camera_file.size();i++)
    {
        if (isupper(camera_file.at(i)))
            throw std::runtime_error("[Optotrak] Camera files on Unix MUST be lowercase");
    }
#endif
    if( OptotrakLoadCameraParameters( optotrakCameraParametersFile ) != OPTO_NO_ERROR_CODE )
    {
        print_errors();
        throw std::runtime_error("[Optotrak] ERROR! An error has occurred during loading camera parameter file, please select a correct filename");
    }
    else
        cerr <<  "\""+string(optotrakCameraParametersFile)+"\" loaded successfully" << endl;

    // Set up a collection for the Optotrak.
    cerr << "[Optotrak] Setup collection..."  << endl;
    if ( OptotrakSetupCollection( NUM_MARKERS, frameRate, markerFreq, DYNAMIC_THRESHOLD,
                                  MINIMUM_GAIN, STREAM_MODE,  dutyCycle, voltage,
                                  COLLECTION_TIME, PRETRIGGER , OPTOTRAK_NO_FIRE_MARKERS_FLAG)
         != OPTO_NO_ERROR_CODE )
    {
        print_errors();
        throw std::runtime_error("[Optotrak] ERROR! An error has occurred during Optotrak Setup System");
    }
    // Wait 2 seconds to let the camera adjust.
    optoTimer.sleep(2000);

    // Activate the markers.
    cerr << "[Optotrak] Optotrak Activate Marker..." << endl;
    if( OptotrakActivateMarkers() != OPTO_NO_ERROR_CODE )
    {
        print_errors();
        throw std::runtime_error("[Optotrak] ERROR! An error has occurred during Optotrak Activate System");
    }

    optoTimer.start();
#endif
    return OPTOTRAK_INIT_SUCCESSFULL;
}

/**
*       \ingroup Optotrak
*       Set the external user-defined transformation, in this case the transformation is only a translation,
*       that is applied to every point in the collection.
*       Remember!! Once this method is called all the points obtained by the getAllMarkers() have this transformation applied!
*
**/
void Optotrak2::setTranslation(const Vector3d &_translation)
{  translation = _translation;
}

/**
*       \ingroup Optotrak
*       Update the points in the collection: read the markers coordinates and copy them in the vector<Vector3d> points.
*
**/
//#define FILTERDATA
void Optotrak2::updateMarkers(double deltaTInMilliSec)
{
#if SIMULATION
    return;
#else
    try
    {  if (p3dData==NULL)
            throw std::runtime_error("[Optotrak] p3dData is NULL, Optotrak not correctly initialized!");
        if ( DataGetNext3D( &uFrameNumber, &uElements, &uFlags, p3dData) )
            throw std::runtime_error("[Optotrak] Cannot collect data at this frame!");
    }
    catch ( std::exception const& e )
    {
        print_errors();
        cerr << e.what() << endl;
        cerr << "[Optotrak] Something wrong happened inside Optotrak::updatePoints:\nPress enter to exit" << endl;
    }

    // Apply translation factor to p3dData before every other computation
    for ( int i=1; i<=NUM_MARKERS; i++)
    {
        p3dData[i-1].x-=translation.coeffRef(0); //X
        p3dData[i-1].y-=translation.coeffRef(1); //Y
        p3dData[i-1].z-=translation.coeffRef(2); //Z
    }
    // Backward difference differentiation scheme
    // Here we implement the velocity computation by using a backward difference schema of 2nd order
    // x'(t) = (3*x(t) - 4*(x(t-1)) + x(t-2) )/ ( 2*DeltaT )
    // velocities are measured in mm/s
    deltaTInMilliSec = optoTimer.getElapsedTimeInMilliSec();
    optoTimer.start();
    double denominator = 0.5/(deltaTInMilliSec*1e-3);

    for ( int i=1; i<=NUM_MARKERS; i++)
    {  points[2][i] = points[1][i];
        points[1][i] = points[0][i];
#ifdef FILTERDATA
        points[0][i].coeffRef(0) = filters[i][0]->Calculate( p3dData[i-1].x );
        points[0][i].coeffRef(1) = filters[i][1]->Calculate( p3dData[i-1].y );
        points[0][i].coeffRef(2) = filters[i][2]->Calculate( p3dData[i-1].z );
#else
        points[0][i].coeffRef(0) =  p3dData[i-1].x ;
        points[0][i].coeffRef(1) =  p3dData[i-1].y ;
        points[0][i].coeffRef(2) =  p3dData[i-1].z ;
#endif
        markers[i].p = points[0][i];
#ifdef FILTERDATA
        Vector3d vtmp = Vector3d::Zero();
        for ( int j=3; j<6; j++ )
        {  vtmp = ((3*points[0][i] - 4*points[1][i] + points[2][i])*(denominator ));
            markers[i].v.coeffRef(j-3) = filters[i][j]->Calculate( vtmp.coeffRef(j-3) );
        }
#else
        markers[i].v = ((3*points[0][i] - 4*points[1][i] + points[2][i])*(denominator));
#endif
    }
#endif
}

/**
*       \ingroup Optotrak
*       Return a reference to the points recorded at the latest frame.
**/
vector<Marker>& Optotrak2::getAllMarkers()
{
    return markers;
}

/**
*       \ingroup Optotrak
*       Return a reference to the points recorded at the latest frame.
**/
vector<Vector3d> &Optotrak2::getAllVelocities()
{
    return velocities;
}

/**
*       \ingroup Optotrak
*       Stop the collection, deactivate the markers, turn off the communication with SCU
**/
void Optotrak2::stopCollection()
{
#if SIMULATION
    return;
#else
    if( OptotrakStopCollection( ) != OPTO_NO_ERROR_CODE )
    {  cerr << "[Optotrak] Stop data collection" << endl;
    }
    OptotrakDeActivateMarkers( );
    TransputerShutdownSystem( );
#endif
}

int Optotrak2::getInternalFrame()
{

    return uFrameNumber;
}
