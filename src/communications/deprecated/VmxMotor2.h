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
#include <string>
#include <boost/thread.hpp>
#include <Eigen/Core>
#include "SerialStream.h"
#include "IOCommon.h"
#include "SerialMotor.h"

/*
\class VmxMotor2
\ingroup Communications
* \brief VmxMotor2 handles the communications through RS232 port to the Velmex Motors
*  C: clear command buffer
*  R: run current commands
*  F: enable RS-232 control (echo off)
*  Q: release RS-232 control
*
*  D: decelerate to stop
*  N: null (zero) position encoders
*  X, Y, Z, T: send position of corresponding axis
*  SmMx: set speed of motor m to x steps/sec
*  ImMx: increment motor m by x steps
*  ImM-x: decrement motor m by x steps
*/

class VmxMotor2 : SerialMotor
{
public:
   VmxMotor2();
   ~VmxMotor2();

   void move3d(int port, int speed, Eigen::Vector3d &delta );
   void move3d(int port, int speed, Eigen::Vector3d &fromPoint, Eigen::Vector3d &toPoint );
   void home3d(int port, int speed);

private:
   // Private constant values
   static const double COM3STEPSIZE, COM4STEPSIZE,COM5STEPSIZE;
   static const int OBJECTCOMPORT=3,MIRRORCOMPORT=4,MONITORCOMPORT=5;

};
