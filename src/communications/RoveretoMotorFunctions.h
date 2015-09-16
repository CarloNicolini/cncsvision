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

#ifndef _ROVERETO_MOTOR_FUNCTIONS_
#define _ROVERETO_MOTOR_FUNCTIONS_
#include <iostream>
#include <boost/thread/thread.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <string>

#include "Util.h"
#include "SerialStream.h"

static const double ROVERETO_MOTORSTEPSIZE=0.005;

#ifdef WIN32
#define OBJECT_SERIAL_PORT "COM3"
#define MIRROR_SERIAL_PORT "COM4"
#define MONITOR_SERIAL_PORT "COM5"
#endif

#ifdef __linux__
#define OBJECT_SERIAL_PORT "/dev/ttyUSB0"
#define MIRROR_SERIAL_PORT "/dev/ttyUSB1"
#define MONITOR_SERIAL_PORT "/dev/ttyUSB2"
#endif

#ifdef __APPLE__
#define OBJECT_SERIAL_PORT ""
#define MIRROR_SERIAL_PORT ""
#define MONITOR_SERIAL_PORT ""
#endif


/**
*
* \ingroup Communication
* \brief This namespace contains functions for moving velmex motors in the Rovereto IIT setup
*
* \namespace RoveretoMotorFunctions
* \brief This namespace contains functions for moving velmex motors in the Rovereto IIT setup
**/
namespace RoveretoMotorFunctions
{

/**
*       \ingroup Communication
*       Move the object connected to serial port #3 to its home and set that home as the zero index start.
*       \param objectspeed Speed of the motors in steps/sec
**/
void homeObject(int objectspeed=2500);

/**
*       \ingroup Communication
*       Move the object connected to serial port #3 to its home and set that home as the zero index start but ASYNCHRONOUSLY.
*       \param objectspeed Speed of the motors in steps/sec
**/
void homeObjectAsynchronous(int objectspeed=2500);

/**
*       \ingroup Communication
*       Move the mirror connected to serial port #4 to its home and set that home as the zero index start.
*       \param mirrorspeed Speed of the mirror in steps/sec
**/
void homeMirror(int mirrorspeed=2500);

/**
*       \ingroup Communication
*       Move the mirror connected to serial port #4 to its home and set that home as the zero index start but ASYNCHRONOUSLY.
*       \param mirrorspeed Speed of the mirror in steps/sec
**/
void homeMirrorAsynchronous(int mirrorspeed=2500);

/**
*       \ingroup Communication
*       Move the screen connected to serial port #5 to its home and set that home as the zero index start.
*       \param screenspeed Speed of the screen in steps/sec
**/
void homeScreen(int screenspeed=2500);

/**
*       \ingroup Communication
*       Move the screen connected to serial port #5 to its home and set that home as the zero index start but ASYNCHRONOUSLY.
*       \param screenspeed Speed of the screen in steps/sec
**/
void homeScreenAsynchronous(int screenspeed=2500);

/**
*       \ingroup Communication
*       Move the object,mirror and screen connected respectively to serial port #3,#4,#5 to their home and set those home as the zero index start.
*       \param objectspeed Speed of the object in steps/sec
*       \param mirrorspeed Speed of the mirror in steps/sec
*       \param screenspeed Speed of the screen in steps/sec
**/
void homeEverything(int objectspeed=2500, int mirrorspeed=2500, int screenspeed=2500);
/**
*       \ingroup Communication
*       Move the object,mirror and screen connected respectively to serial port #3,#4,#5 to their home and set those home as the zero index start.
*       \param objectspeed Speed of the object in steps/sec
*       \param mirrorspeed Speed of the mirror in steps/sec
*       \param screenspeed Speed of the screen in steps/sec
**/
void homeEverythingAsynchronous(int objectspeed=2500, int mirrorspeed=2500, int screenspeed=2500);

/******** ABSOLUTE MOVEMENT FUNCTIONS ********/

/**
*       \ingroup Communication
*       Move the object on serial port #3 in absolute coordinates, given that a coherent index was set (this is done via homeObject function)
*       \param point 3D position to reach
*       \param calibration Point in 3d space representing a generic point lying on the object when is at home position
*       \param objectspeed Speed of the object in steps/sec
**/
void moveObjectAbsolute(const Eigen::Vector3d &point, const Eigen::Vector3d &calibration, int objectspeed=2500);

/**
*       \ingroup Communication
*       Move the object on serial port #3 in relative coordinates, given that a coherent index was set (this is done via homeObject function)
*       \param delta displacement in millimeters
*       \param objectspeed Speed of the object in steps/sec
**/
void moveObject(const Eigen::Vector3d &delta, int objectspeed=2500);

/**
*       \ingroup Communication
*       Move the object on serial port #3 in absolute coordinates, given that a coherent index was set (this is done via homeObject function) but ASYNCHRONOUSLY
*       \param point 3D position to reach
*       \param calibration Point in 3d space representing a generic point lying on the object when is at home position
*       \param objectspeed Speed of the object in steps/sec
**/
void moveObjectAbsoluteAsynchronous(const Eigen::Vector3d &point, const Eigen::Vector3d &calibration, int objectspeed=2500);

/**
*       \ingroup Communication
*       Move the mirror on serial port #4 in absolute coordinates, given that a coherent index was set (this is done via homeMirror function)
*       \param point 3D position to reach (y coordinates is ignored because no y translations are allowed in this setup for the mirror)
*       \param calibration Point in 3d space representing a generic point lying on the mirror when is at home position
*       \param mirrorspeed Speed of the object in steps/sec
**/
void moveMirrorAbsolute(const Eigen::Vector3d &point, const Eigen::Vector3d &calibration, int mirrorspeed=2500);

/**
*       \ingroup Communication
*       Move the mirror on serial port #4 in absolute coordinates, given that a coherent index was set (this is done via homeMirror function) but ASYNCHRONOUSLY
*       \param point 3D position to reach (y coordinates is ignored because no y translations are allowed in this setup for the mirror)
*       \param calibration Point in 3d space representing a generic point lying on the mirror when is at home position
*       \param objectspeed Speed of the mirror in steps/sec
**/
void moveMirrorAbsoluteAsynchronous(const Eigen::Vector3d &point, const Eigen::Vector3d &calibration, int objectspeed=2500);

/**
*       \ingroup Communication
*       Move the screen on serial port #5 in absolute coordinates, given that a coherent index was set (this is done via homeObject function) but ASYNCHRONOUSLY
*       \param focalDistance Final focal distance in the current reference frame system.
*       \param homeFocalDistance Focal distance when the screen is at home position in the current reference frame system.
*       \param screenspeed Speed of the screen in steps/sec
**/
void moveScreenAbsolute(double focalDistance, double homeFocalDistance, int screenspeed=2500);

/**
*       \ingroup Communication
*       Move the screen on serial port #5 in absolute coordinates, given that a coherent index was set (this is done via homeObject function) but ASYNCHRONOUSLY
*       \param focalDistance Final focal distance in the current reference frame system.
*       \param homeFocalDistance Focal distance when the screen is at home position in the current reference frame system.
*       \param screenspeed Speed of the screen in steps/sec
**/
void moveScreenAbsoluteAsynchronous(double focalDistance, double homeFocalDistance, int screenspeed=2500);

// STEPS VALUE READING FUNCTIONS
/**
*       \ingroup Communication
*       Ask the velmex system the absolute index of the current 1,2,3 motors connected on serial port #3 (object) (should be (0,0,0) when called after a homeObject() )
*       \return Index steps values
**/
Eigen::Vector3i readObjectPositions();

/**
*       \ingroup Communication
*       Ask the velmex system the absolute index of the current 1,2 motors connected on serial port #4 (mirror) (should be (0,0,0) when called after a homeMirror() )
*       \return Index steps values
**/
Eigen::Vector3i readMirrorPositions();

/**
*       \ingroup Communication
*       Ask the velmex system the absolute index of the current 1,2 motors connected on serial port #5 (screen) (should be (0,0,0) when called after a homeScreen() )
*       \return Index steps values
**/
Eigen::Vector3i readScreenPositions();
}

#endif
