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


#include "RoveretoMotorFunctions.h"

using namespace std;
using namespace Eigen;
using util::stringify;
using util::str2num;


namespace RoveretoMotorFunctions
{
void printString(const std::string &buffer)
{
#ifdef DEBUG
   cerr << "Sending:\"" << buffer << "\"... "  << endl;
#endif
}

void homeObject(int objectspeed)
{  string buffer = string("F,C,")+string("S1M")+stringify<int>(objectspeed)+string(",S2M")+stringify<int>(objectspeed)+string(",S3M")+stringify<int>(objectspeed)+
                   string(",(I1M-0,I3M-0,)I2M0,IA1M-0,IA2M-0,IA3M-0,R");
   printString(buffer);
   SerialStream serial(OBJECT_SERIAL_PORT);
   serial << buffer << endl;
   char out=0;
   serial >> out;
   if ( out!='^' )
      cerr << "Error in RoveretoMotorFunctions::homeObject()" << endl;
   char out2[10];
   int x=INT_MAX,y=INT_MAX,z=INT_MAX;
   serial << "X" << endl;
   serial >> out2;
   z = str2num<int>(string(out2));
   serial << "Y" << endl;
   serial >> out2;
   y = str2num<int>(string(out2));
   serial << "Z" << endl;
   serial >> out2;
   x = str2num<int>(string(out2));

}

void homeObjectAsynchronous(int objectspeed)    //XXX
{  boost::thread motorthread1(boost::bind(&homeObject,objectspeed));
}

void homeMirror(int mirrorspeed) //XXX
{  string buffer = string("F,C,")+string("S1M")+stringify<int>(mirrorspeed)+string(",S2M")+stringify<int>(mirrorspeed)+
                   string(",I1M0,I2M0,IA1M-0,IA2M-0,R");
   printString(buffer);

   SerialStream serial(MIRROR_SERIAL_PORT);
   serial << buffer << endl;
   char out=0;
   serial >> out;
   if ( out!='^' )
      cerr << "Error in RoveretoMotorFunctions::homeMirror()" << endl;
}

void homeMirrorAsynchronous(int mirrorspeed) //XXX
{  boost::thread motorthread1(boost::bind(&homeMirror,mirrorspeed));
}

void homeScreen(int screenspeed) //XXX
{  string buffer = string("F,C,")+string("S1M")+stringify<int>(screenspeed)+string(",S2M")+stringify<int>(screenspeed)+
                   string(",I1M0,I2M0,IA1M-0,IA2M-0,R");

   printString(buffer);

   SerialStream serial(MONITOR_SERIAL_PORT); //open a standard serial port, baudrate 9600
   serial << buffer << endl;
   char out=0;
   serial >> out;
   if ( out!='^' )
      cerr << "Error in RoveretoMotorFunctions::homeScreen()" << endl;
}

void homeScreenAsynchronous(int screenspeed) //XXX
{  boost::thread motorthread1(boost::bind(&homeScreen,screenspeed));
}

void homeEverything(int objectspeed, int mirrorspeed, int screenspeed) //XXX
{  homeObject(objectspeed);
   homeMirror(mirrorspeed);
   homeScreen(screenspeed);
}

void homeEverythingAsynchronous(int objectspeed, int mirrorspeed, int screenspeed) //XXX
{  boost::thread motorthread1(boost::bind(&homeObject,objectspeed));
   boost::thread motorthread2(boost::bind(&homeMirror,mirrorspeed));
   boost::thread motorthread3(boost::bind(&homeScreen,screenspeed));
}

void moveObjectAbsolute(const Eigen::Vector3d &point, const Eigen::Vector3d &calibration, int objectspeed)
{  int nstepsX = -(int) (( point.x()-calibration.x())/ROVERETO_MOTORSTEPSIZE);
   int nstepsY = -(int) (( point.y()-calibration.y())/ROVERETO_MOTORSTEPSIZE);
   int nstepsZ = (int)  (( point.z()-calibration.z())/ROVERETO_MOTORSTEPSIZE);

   string buffer =
      string("F,C,")+
      string("S1M")+stringify<int>(objectspeed)+
      string(",S2M")+stringify<int>(objectspeed)+
      string(",S3M")+stringify<int>(objectspeed)+
      string(",(IA3M")+stringify<int>(nstepsX)+
      string(",IA1M")+stringify<int>(nstepsZ)+
      string(",)IA2M")+stringify<int>(nstepsY)+
      string(",R");
   printString(buffer);

   SerialStream serial(OBJECT_SERIAL_PORT); //open a standard serial port, baudrate 9600
   serial << buffer << endl;
   char out=0;
   serial >> out;
   if ( out!='^' )
      cerr << "Error in RoveretoMotorFunctions::moveObjectAbsolute()" << endl;
}

void moveObject(const Eigen::Vector3d &delta, int objectspeed)
{  int nstepsX = -(int) ( delta.x()/ROVERETO_MOTORSTEPSIZE);
   int nstepsY = -(int) ( delta.y()/ROVERETO_MOTORSTEPSIZE);
   int nstepsZ = (int)  ( delta.z()/ROVERETO_MOTORSTEPSIZE );

   nstepsX==0 ? nstepsX=1 : nstepsX=nstepsX;
   nstepsY==0 ? nstepsY=1 : nstepsY=nstepsY;
   nstepsZ==0 ? nstepsZ=1 : nstepsZ=nstepsZ;

   string buffer =
      string("F,C,")+
      string("S1M")+stringify<int>(objectspeed)+
      string(",S2M")+stringify<int>(objectspeed)+
      string(",S3M")+stringify<int>(objectspeed)+
      string(",(I3M")+stringify<int>(nstepsX)+
      string(",I1M")+stringify<int>(nstepsZ)+
      string(",)I2M")+stringify<int>(nstepsY)+
      string(",R");
   printString(buffer);

   SerialStream serial(OBJECT_SERIAL_PORT); //open a standard serial port, baudrate 9600
   serial << buffer << endl;
   char out=0;
   serial >> out;
   if ( out!='^' )
      cerr << "Error in RoveretoMotorFunctions::moveObject()" << endl;
}

void moveMirrorAbsolute(const Eigen::Vector3d &point, const Eigen::Vector3d &calibration, int speed)
{  int nstepsX = -(int) ( point.x()-calibration.x()/ROVERETO_MOTORSTEPSIZE);
   int nstepsZ = (int)  ( point.z()-calibration.z()/ROVERETO_MOTORSTEPSIZE);
   string buffer =
      string("F,C,")+
      string("S1M")+stringify<int>(speed)+
      string(",S2M")+stringify<int>(speed)+
      string(",IA1M")+stringify<int>(nstepsZ)+
      string(",IA2M")+stringify<int>(nstepsX)+
      string(",R");

   printString(buffer);

   SerialStream serial(MIRROR_SERIAL_PORT); //open a standard serial port, baudrate 9600
   serial << buffer << endl;
   char out=0;
   serial >> out;
   if ( out!='^' )
      cerr << "Error in RoveretoMotorFunctions::moveMirrorAbsolute()" << endl;
}

void moveMirrorAbsoluteAsynchronous(const Eigen::Vector3d &point, const Eigen::Vector3d &calibration, int speed)
{  boost::thread motorthread1(boost::bind(&moveMirrorAbsolute,point,calibration,speed));
}

void moveObjectAbsoluteAsynchronous(const Eigen::Vector3d &point, const Eigen::Vector3d &calibration, int objectspeed)
{  boost::thread motorthread1(boost::bind(&moveObjectAbsolute,point,calibration,objectspeed));
}

void moveScreenAbsolute(double focalDistance, double homeFocalDistance, int screenspeed)
{  int nstepsX = (int)((focalDistance-homeFocalDistance)/(ROVERETO_MOTORSTEPSIZE));
   if ( nstepsX==0)
      return;
   string buffer =
      string("F,C,")+
      string("S2M")+stringify<int>(screenspeed)+
      string(",IA2M")+stringify<int>(nstepsX)+
      string(",R");
   printString(buffer);
   SerialStream serial(MONITOR_SERIAL_PORT);
   serial << buffer << endl;
   char out=0;
   serial >> out;
   if ( out!='^' )
      cerr << "Error in RoveretoMotorFunctions::moveScreenAbsolute()" << endl;
}

void moveScreenAbsoluteAsynchronous(double focalDistance, double homeFocalDistance, int screenspeed)
{  boost::thread motorthread1(boost::bind(&moveScreenAbsolute,focalDistance,homeFocalDistance,screenspeed));
}

Eigen::Vector3i readObjectPositions()
{  SerialStream serial(OBJECT_SERIAL_PORT);
   // Home object on Z axis
   string Z,X,Y;
   serial << "Z" << endl;
   serial >> Z;

   serial << "X" << endl;
   serial >> X;

   serial << "Y" << endl;
   serial >> Y;
   double x = str2num<int>(Z)*ROVERETO_MOTORSTEPSIZE;
   double y = str2num<int>(X)*ROVERETO_MOTORSTEPSIZE;
   double z = str2num<int>(Y)*ROVERETO_MOTORSTEPSIZE;

   Eigen::Vector3i positionIntegers( str2num<int>(Z),str2num<int>(X),str2num<int>(Y) );
   Eigen::Vector3d position(x,y,z);
   cerr << "Absolute position" << position.transpose() << "[mm]" << endl;
   return positionIntegers;
}

Eigen::Vector3i readMirrorPositions()
{  SerialStream serial(MIRROR_SERIAL_PORT);

// Home object on Z axis
   string Z,X,Y;
   serial << "Z" << endl;
   serial >> Z;

   serial << "X" << endl;
   serial >> X;

   serial << "Y" << endl;
   serial >> Y;
   double x = str2num<int>(Z)*ROVERETO_MOTORSTEPSIZE;
   double y = str2num<int>(X)*ROVERETO_MOTORSTEPSIZE;
   double z = str2num<int>(Y)*ROVERETO_MOTORSTEPSIZE;

   Eigen::Vector3i positionIntegers( str2num<int>(Z),str2num<int>(X),str2num<int>(Y) );
   Eigen::Vector3d position(x,y,z);
   cerr << "Absolute position" << position.transpose() << "[mm]" << endl;
   return positionIntegers;
}

Eigen::Vector3i readScreenPositions()
{  SerialStream serial(MONITOR_SERIAL_PORT);

// Home object on Z axis
   string Z,X,Y;
   serial << "Z" << endl;
   serial >> Z;

   serial << "X" << endl;
   serial >> X;

   serial << "Y" << endl;
   serial >> Y;
   double x = str2num<int>(Z)*ROVERETO_MOTORSTEPSIZE;
   double y = str2num<int>(X)*ROVERETO_MOTORSTEPSIZE;
   double z = str2num<int>(Y)*ROVERETO_MOTORSTEPSIZE;

   Eigen::Vector3i positionIntegers( str2num<int>(Z),str2num<int>(X),str2num<int>(Y) );
   Eigen::Vector3d position(x,y,z);
   cerr << "Absolute position" << position.transpose() << "[mm]" << endl;
   return positionIntegers;
}
}
