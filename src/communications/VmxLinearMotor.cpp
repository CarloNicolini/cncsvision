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

#include "SerialMotor.h"
#include "VmxLinearMotor.h"

using namespace std;
using util::stringify;
using util::str2num;

const int VmxLinearMotor::MOTOR_ID_OBJECT_X=3;
const int VmxLinearMotor::MOTOR_ID_OBJECT_Y=2;
const int VmxLinearMotor::MOTOR_ID_OBJECT_Z=1;

const int VmxLinearMotor::MOTOR_ID_SCREEN_X=2;
const int VmxLinearMotor::MOTOR_ID_SCREEN_Z=1;

const int VmxLinearMotor::MOTOR_ID_MIRROR_X=2;
const int VmxLinearMotor::MOTOR_ID_MIRROR_Z=1;


const double VmxLinearMotor::COM3STEPSIZE=5E-3f;
const double VmxLinearMotor::COM4STEPSIZE=5E-3f;
const double VmxLinearMotor::COM5STEPSIZE=5E-3f;

/** \ingroup Communication
* Constructor of VmxLinearMotor object
* \param _comport The serial port index, can be 3,4,5
* \param _baudrate The baudrate of the port
**/
VmxLinearMotor::VmxLinearMotor(int _comport,int _baudrate)
{  port=_comport;
   baudrate=_baudrate;
}

/** \ingroup Communication
* Move the motor on selected port of a displacement delta
* \param port The serial port index, can be 3,4,5
* \param delta The displacement in millimeters
* \param speed The speed of motor
**/
void VmxLinearMotor::move3d(int port, Eigen::Vector3d &delta, int speed )
{  if ( port < 3 || port > 5 )
      throw std::range_error(string("WARNING! This port doesn't represent any valid Velmex motor, select ports in the range [3,5]"));
   if ( delta.norm() > 1E9 )
      throw std::range_error(string("WARNING! You're trying to move a motor to the infinity! Check consistency of your input"));

   clearBuffer();
   try
   {  init(port);
   }
   catch (...) {}

   int nstepsX=0, nstepsY=0, nstepsZ=0;

   // Create the string to send via serial port
   switch ( port )
   {  case MONITORCOMPORT:
      {  //int motorXID=2, motorZID=1;
         nstepsX= (int) floor( delta.x()/COM5STEPSIZE );
         nstepsZ=  (int) floor( delta.z()/COM5STEPSIZE );
         if ( nstepsX==0 )
            nstepsX+=1;
         if ( nstepsZ==0 )
            nstepsZ+=1;

         // X Movements
         buffer="F,C,S"  +stringify<int>(MOTOR_ID_SCREEN_X)+"M"+stringify<int>(speed)+",S"+stringify<int>(MOTOR_ID_SCREEN_Z)+"M"+stringify<int>(speed)+
                ",I"+stringify<int>(MOTOR_ID_SCREEN_X)+"M"+stringify<int>(nstepsX);

         // Z Movements
         buffer+=         ",I"+stringify<int>(MOTOR_ID_SCREEN_Z)+"M"+stringify<int>(nstepsZ);
         // command termination
         buffer+=",R";
      }
      break;
      case MIRRORCOMPORT:
      {  nstepsX= -(int) floor( delta.x()/COM4STEPSIZE );
         nstepsZ= (int) floor( delta.z()/COM4STEPSIZE );
         if ( nstepsX==0 )
            nstepsX+=1;
         if ( nstepsZ==0 )
            nstepsZ+=1;

         // X Movements
         buffer="F,C,S"  +stringify<int>(MOTOR_ID_MIRROR_X)+"M"+stringify<int>(speed)+",S"+stringify<int>(MOTOR_ID_MIRROR_Z)+"M"+stringify<int>(speed)+
                ",I"+stringify<int>(MOTOR_ID_MIRROR_X)+"M"+stringify<int>(nstepsX);
         // Z Movements
         buffer+=         ",I"+stringify<int>(MOTOR_ID_MIRROR_Z)+"M"+stringify<int>(nstepsZ);
         // command termination
         buffer+=",R";
      }
      break;
      case OBJECTCOMPORT:
      {  nstepsX= -(int) floor( delta.x()/COM3STEPSIZE );   // axis x and y are reversed w.r.t frame of reference
         nstepsY= - (int) floor( delta.y()/COM3STEPSIZE );
         nstepsZ= (int) floor( delta.z()/COM3STEPSIZE );
         if ( nstepsX==0 )
            nstepsX+=1;
         if ( nstepsY==0 )
            nstepsY+=1;
         if ( nstepsZ==0 )
            nstepsZ+=1;

         // X Movements
         buffer="F,C,(S"  +stringify<int>(MOTOR_ID_OBJECT_X)+"M"+stringify<int>(speed)+",S"+stringify<int>(MOTOR_ID_OBJECT_Y)+"M"+stringify<int>(speed)+",S"+stringify<int>(MOTOR_ID_OBJECT_Z)+"M"+stringify<int>(speed)+
                ",I"+stringify<int>(MOTOR_ID_OBJECT_X)+"M"+stringify<int>(nstepsX);
         // Y Movements
         buffer+=        ",I"+stringify<int>(MOTOR_ID_OBJECT_Y)+"M"+stringify<int>(nstepsY);
         // Z Movements
         buffer+=         ",I"+stringify<int>(MOTOR_ID_OBJECT_Z)+"M"+stringify<int>(nstepsZ);
         // command termination
         buffer+=",)R";
      }
      break;
   };
   write();
}

/** \ingroup Communication
* Move the motors on specified port with a 3 dimensional (or 2 dimensional) translation vector with the same frame of global system
* \param port The serial port index, can be {3,4,5}
* \param speed The speed of movements in steps/second
* \param fromPosition Starting position in millimeters (if this is known)
* \param toPosition End position in millimiters
**/
void VmxLinearMotor::move3d(int port, Eigen::Vector3d &fromPosition , Eigen::Vector3d &toPosition,  int speed )
{  Eigen::Vector3d delta = toPosition-fromPosition;
   move3d(port,delta, speed);
}

/** \ingroup Communication
* Move the specified motor to its limit switch
* Upper bound on speed while homing is set to 3000 steps/sec
* \param port The serial port index, can be {3,4,5}
* \param speed The speed of movements in steps/second
**/
void VmxLinearMotor::home3d(int port, int speed)
{  speed > 3500 ? speed=3500 : speed=speed;

   string speedstring = stringify<int>(speed);
   clearBuffer();
   init(port);

   /* Create the string to send via serial port */
   switch ( port )
   {  case MONITORCOMPORT:
      case MIRRORCOMPORT: //for a pure case they share the same motor order...
      {  buffer=string("F,C,")+string("S1M")+stringify<int>(speed)+string(",S2M")+stringify<int>(speed)+
                string(",I1M0,I2M0,IA1M-0,IA2M-0,R");
      }
      break;
      case OBJECTCOMPORT:
      {  string stringspeed=stringify<int>(speed);
         buffer="F,C,S1M"+stringspeed+",S2M"+stringspeed+",S3M"+stringspeed+",I3M-0,I2M0,I1M-0,R";
      }
      break;
   };
   write();
}

/** \ingroup Communication
* Move the motor on a given port with given id of a certain number of steps
* \param port The serial port index, can be {3,4,5}
* \param motorid The id of the motor (it's written on the cables typically)
* \param steps The number of steps to move the motor
* \param speed The speed of movements in steps/second
**/
void VmxLinearMotor::moveSteps(int port, int motorid, int steps, int speed)
{  clearBuffer();
   init(port);
   if (steps==0)
      return;

   string smotorID=stringify<int>(motorid);
   string ssteps=stringify<int>(steps);
   string sspeed = stringify<int>(speed);

   buffer="F,C,S" + smotorID + "M" + sspeed + ",I" + smotorID +"M" + ssteps + ",R";
   write();
}

void VmxLinearMotor::moveSteps(int port, int motorid, float millimeters, int speed, float stepsize)
{  clearBuffer();
   init(port);

   string smotorID=stringify<int>(motorid);
   int steps = millimeters/stepsize;
   if (steps==0)
      return;
   string ssteps=stringify<int>(steps);
   string sspeed = stringify<int>(speed);

   buffer="F,C,S" + smotorID + "M" + sspeed + ",I" + smotorID +"M" + ssteps + ",R";

   write();
}

