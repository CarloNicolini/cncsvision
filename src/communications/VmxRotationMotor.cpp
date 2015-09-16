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


#include "VmxRotationMotor.h"

using namespace std;
using namespace boost;
using util::stringify;
using util::str2num;

/** \ingroup Communication
* Default constructor
**/
VmxRotationMotor::VmxRotationMotor(int _communicationPort, int _motorID)
{  port=_communicationPort;
   motorID=_motorID;
   init(port,baudrate);
}
/**
* \ingroup Communication
* Rotate the motor
* \param steps number of steps
* \param speed speed in steps/sec
**/
void VmxRotationMotor::rotate( int steps, int speed )
{  clearBuffer();
   init(port);


   buffer="F,C,S"+stringify<int>(motorID)+"M"+stringify<int>(speed)+",I"+stringify<int>(motorID)+"M"+stringify<int>(steps)+",R";
   cerr << buffer << endl;

   write();     //method of father class SerialMotor::write();
}

/**
* \ingroup Communication
* Rotate the motor
* \param angle angle in degrees [0,360]
* \param speed speed in steps/sec
**/
void VmxRotationMotor::rotate( double angle, int speed)
{  int steps = floor(angle*100);
   rotate((int)steps,speed);
}

