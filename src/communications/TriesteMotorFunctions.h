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

#ifndef _TRIESTE_MOTOR_FUNCTIONS_
#define _TRIESTE_MOTOR_FUNCTIONS_
#include <iostream>
#include <boost/thread/thread.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <string>

#include "Util.h"
#include "SerialStream.h"

static const double TRIESTE_MOTORSTEPSIZE=0.005;

/**
*
* \ingroup Communication
* \brief This namespace contains functions for moving velmex motors in the TRIESTE IIT setup
*
* \namespace TriesteMotorFunctions
* \brief This namespace contains functions for moving velmex motors in the TRIESTE IIT setup
**/
namespace TriesteMotorFunctions
{
void sendCommand(int port, const string &command);
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
*       Move the Monitor connected to serial port #5 to its home and set that home as the zero index start.
*       \param Monitorspeed Speed of the Monitor in steps/sec
**/
void homeMonitor(int monitorspeed=2500);

/**
*       \ingroup Communication
*       Move the Monitor connected to serial port #5 to its home and set that home as the zero index start but ASYNCHRONOUSLY.
*       \param Monitorspeed Speed of the Monitor in steps/sec
**/
void homeMonitorAsynchronous(int monitorspeed=2500);

/**
*       \ingroup Communication
*       Move the object,mirror and Monitor connected respectively to serial port #3,#4,#5 to their home and set those home as the zero index start.
*       \param objectspeed Speed of the object in steps/sec
*       \param mirrorspeed Speed of the mirror in steps/sec
*       \param Monitorspeed Speed of the Monitor in steps/sec
**/
void homeEverything(int objectspeed=2500, int mirrorspeed=2500, int Monitorspeed=2500);
/**
*       \ingroup Communication
*       Move the object,mirror and Monitor connected respectively to serial port #3,#4,#5 to their home and set those home as the zero index start.
*       \param objectspeed Speed of the object in steps/sec
*       \param mirrorspeed Speed of the mirror in steps/sec
*       \param Monitorspeed Speed of the Monitor in steps/sec
**/
void homeEverythingAsynchronous(int objectspeed=2500, int mirrorspeed=2500, int Monitorspeed=2500);

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
*  \ingroup Communication
 * @brief moveMirrorRelative
 * @param deltaz
 * Move the mirror by a fixed amount along z
 */
void moveMirror(double deltaz, double speed=2500);

/**
*       \ingroup Communication
*       Move the mirror on serial port #4 in absolute coordinates, given that a coherent index was set (this is done via homeMirror function)
*       \param finalFocalDistance position to reach
*       \param baseFocalDistance representing the focal distance of monitor at home
*       \param mirrorspeed Speed of the object in steps/sec
**/
void moveMirrorAbsolute(double finalFocalDistance, double baseFocalDistance, int mirrorspeed=2500);

/**
*       \ingroup Communication
*       Move the mirror on serial port #4 in absolute coordinates, given that a coherent index was set (this is done via homeMirror function) BUT ASYNCHRONOUSLY
*       \param finalFocalDistance position to reach
*       \param baseFocalDistance representing the focal distance of monitor at home
*       \param mirrorspeed Speed of the object in steps/sec
**/
void moveMirrorAbsoluteAsynchronous(double finalFocalDistance, double baseFocalDistance, int speed);

/**
*       \ingroup Communication
*       Move the Monitor on serial port #5 in absolute coordinates, given that a coherent index was set (this is done via homeObject function) but ASYNCHRONOUSLY
*       \param focalDistance Final focal distance in the current reference frame system.
*       \param homeFocalDistance Focal distance when the Monitor is at home position in the current reference frame system.
*       \param Monitorspeed Speed of the Monitor in steps/sec
**/
void moveMonitorAbsolute(double focalDistance, double homeFocalDistance, int Monitorspeed=2500);

/**
*       \ingroup Communication
*       Move the Monitor on serial port #5 in absolute coordinates, given that a coherent index was set (this is done via homeObject function) but ASYNCHRONOUSLY
*       \param focalDistance Final focal distance in the current reference frame system.
*       \param homeFocalDistance Focal distance when the Monitor is at home position in the current reference frame system.
*       \param Monitorspeed Speed of the Monitor in steps/sec
**/
void moveMonitorAbsoluteAsynchronous(double focalDistance, double homeFocalDistance, int Monitorspeed=2500);
}

#endif
