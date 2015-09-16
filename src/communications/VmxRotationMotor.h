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

#ifndef _VMX_ROTATION_MOTOR_H_
#define _VMX_ROTATION_MOTOR_H_

#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <Eigen/Core>
#include "SerialStream.h"
#include "SerialMotor.h"

/*
\class VmxRotationMotor
\ingroup Communication
* \brief VmxRotationMotor handles the communications through RS232 port to the Velmex Motors
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

class VmxRotationMotor : public SerialMotor
{
public:
   VmxRotationMotor(int comport=3, int motorID=4);

   void rotate(double angle, int speed=2000);
   void rotate(int steps, int speed=2000);
   void setZero();

   int motorID;
   static const double ANGLESTEPSIZE;
   static const int OBJECTCOMPORT=3,MIRRORCOMPORT=4,MONITORCOMPORT=5;

};

static const double ANGLESTEPSIZE=0.01;

#endif
