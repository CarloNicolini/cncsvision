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

#ifndef _SERIAL_MOTOR_H
#define _SERIAL_MOTOR_H

#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <Eigen/Core>
#include "BufferedAsyncSerial.h"
#include "AsyncSerial.h"
#include "SerialStream.h"
#include "Util.h"

/**
* \defgroup Communication
* \brief The group communication contains all the classes, utilities and namespace to handle communication with external devices basically using serial port.
* Two important namespaces are contained, the RoveretoMotorFunctions and the BrownMotorFunctions which are needed to work with the motors in the Rovereto and Providence laboratories.
* The functions in these namespace move typically linear velmex motors sending short strings via COM ports (on windows machines) and they present in two flavors,
* the normal blocking calls (they finish when the motors has finished moving and the asynchronous calls which returns suddenly. It's up to the experimenter to choose which version use.
* The main drawback of the blocking calls is that it's impossible to move two motors on differents COM ports together. This is possible with asynchronous calls.
* The asynchronous calls instead must be accurately timed because if two asynchronous functions write on the same port they crash (impossible to split the resource via hardware) and a exception is thrown.
**/

/*
\class SerialMotor
\ingroup Communication
* \brief VmxMotor handles the communications through RS232 port to the Velmex Motors
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

class SerialMotor
{
public:
   SerialMotor(int _comport=1, int _baudrate=9600);
   ~SerialMotor();
   void init( int _serialportnumber, int baudrate=9600 );
   //void init( string _communicationPort, int baudrate=9600 );
   void write();
   void clearBuffer();
   void dispose();

protected:
   int port,baudrate;
   string buffer, communicationPort;
};

#endif
