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

#ifndef _BROWN_MOTOR_FUNCTIONS_
#define _BROWN_MOTOR_FUNCTIONS_
#include <iostream>
#include <boost/thread/thread.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <string>

#include "Util.h"
#include "SerialStream.h"
#include "VmxLinearMotor.h"


using util::stringify;
using util::str2num;

#define BROWN_MOTORS_COM_PORT 3
#define BROWN_OBJECT_Y_AXIS 1 //asse discorde serve segno - davanti
#define BROWN_OBJECT_Z_AXIS 2 //asse concorde
#define BROWN_SCREEN_X_AXIS 3 //asse concorde
static const double BROWN_MOTORSTEPSIZE=0.005f;

/**
*
* \ingroup Communication
* \brief This namespace contains functions for moving velmex motors in the Brown university setup
*
* \namespace BrownMotorFunctions
* \brief This namespace contains functions for moving velmex motors in the Brown university setup
**/

namespace BrownMotorFunctions
{
// HOMING FUNCTIONS
void homeObject(int objectspeed=4000);
void homeObjectAsynchronous(int objectspeed=5000);
void homeScreen(int screenspeed=3500);
void homeScreenAsynchronous(int screenspeed=3500);
void homeEverything(int objectspeed, int screenspeed);
void homeEverythingAsynchronous(int objectspeed, int screenspeed);

// ABSOLUTE MOVEMENT FUNCTIONS
void moveObjectAbsolute(const Eigen::Vector3d &point, const Eigen::Vector3d &calibration, int objectspeed);
void moveScreenAbsolute(double focalDistance, double homeFocalDistance, int screenspeed);
void moveScreenAbsoluteAsynchronous(double focalDistance, double homeFocalDistance, int screenspeed);


// RELATIVE MOVEMENT FUNCTIONS
void moveObject(const Eigen::Vector3d &delta, int speed);
void moveObjectAsynchronous(const Eigen::Vector3d &delta, int speed);

void moveObject(const Eigen::Vector3d &from, const Eigen::Vector3d &to, int speed);
void moveObjectAsynchronous(const Eigen::Vector3d &from, const Eigen::Vector3d &to, int speed);

void moveScreen(const Eigen::Vector3d &delta, int speed);
void moveScreenAsynchronous(const Eigen::Vector3d &delta, int speed);

void moveScreen(const Eigen::Vector3d &from, const Eigen::Vector3d &to, int speed);
void moveScreenAsynchronous(const Eigen::Vector3d &from, const Eigen::Vector3d &to, int speed);

void moveObjectAndScreen(const Eigen::Vector3d &objectDelta, const Eigen::Vector3d &screenDelta, int objectspeed, int screenspeed=3500 );
void moveObjectAndScreenAsynchronous(const Eigen::Vector3d &objectDelta,const Eigen::Vector3d &screenDelta, int objectspeed,int screenspeed=3500 );

void moveObjectAndScreen(const Eigen::Vector3d &objectFrom,const Eigen::Vector3d &objectTo,const Eigen::Vector3d &screenFrom,const Eigen::Vector3d &screenTo,int objectspeed,int screenspeed=3500 );
void moveObjectAndScreenAsynchronous(const Eigen::Vector3d &objectFrom, const Eigen::Vector3d &objectTo,const Eigen::Vector3d &screenFrom, const Eigen::Vector3d &screenTo, int objectspeed, int screenspeed=3500 );

// STEPS VALUE READING FUNCTIONS
Eigen::Vector3i readPositions();
}
#endif
