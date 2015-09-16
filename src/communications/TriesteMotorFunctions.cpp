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


#include "TriesteMotorFunctions.h"

using util::stringify;
using util::str2num;

// The mapping is the following:
// COM3:    Monitor 1->x    s+/z+
// COM3:    Monitor 2->z    s+/z+

// COM5:    Mirror 1 -> z   s+/z+
// COM5     Object 2 -> x   s+/x-
// COM5:    Object 3 -> y   s+/y-
// COM5:    Object4 ->  z   s+/z+

namespace TriesteMotorFunctions
{
void sendCommand(int port, const string &command)
{  string serialportstr = "COM" + util::stringify<int>(port);
#ifdef _WIN32
   SerialStream serial(serialportstr);
   serial << command << endl;

   char out=0;
   serial >> out;
   if ( out!='^' )
      cerr << "Error in sendmotorCommands()" << endl;
#else
   cerr << "Sending " << command << " to " << serialportstr << endl;
#endif
}

void homeObject(int objectspeed)
{  /// 2= x; 3= y; 4= z
   int s = (objectspeed > 4000 ? 4000 : objectspeed);
   string speed=stringify<int>(s);
   string buffer = string("F,C,")+string("S2M")+speed+","+
                   string("S3M")+speed+","+
                   string("S4M")+speed+"," +
                   string( "I2M-0,I3M0,I4M-0,IA2M-0,IA3M-0,IA4M-0,R");
   sendCommand(5,buffer);
}

void homeObjectAsynchronous(int objectspeed)
{  boost::thread motorthread1(boost::bind(&homeObject,objectspeed));
}

void homeMirror(int mirrorspeed)
{  string buffer = string("F,C,")+string("S1M")+stringify<int>(mirrorspeed)+
                   string(",I1M0,IA1M-0,R");

   sendCommand(5,buffer);
}

void homeMirrorAsynchronous(int mirrorspeed)
{  boost::thread motorthread1(boost::bind(&homeMirror,mirrorspeed));
}

void homeMonitor(int monitorspeed)
{  string buffer = string("F,C,")+string("S1M")+stringify<int>(monitorspeed)+string(",S2M")+stringify<int>(monitorspeed)+
                   string(",I1M0,I2M0,IA1M-0,IA2M-0,R");

   sendCommand(3,buffer);
}

void homeMonitorAsynchronous(int monitorspeed)
{  boost::thread motorthread1(boost::bind(&homeMonitor,monitorspeed));
}

void homeEverything(int objectspeed, int mirrorspeed, int Monitorspeed)
{  homeObject(objectspeed);
   homeMirror(mirrorspeed);
   homeMonitor(Monitorspeed);
}

void homeEverythingAsynchronous(int objectspeed, int mirrorspeed, int Monitorspeed)
{  boost::thread motorthread1(boost::bind(&homeObject,objectspeed));
   boost::thread motorthread2(boost::bind(&homeMirror,mirrorspeed));
   boost::thread motorthread3(boost::bind(&homeMonitor,Monitorspeed));
}

void moveObjectAbsolute(const Eigen::Vector3d &point, const Eigen::Vector3d &calibration, int objectspeed)
{  int nstepsX = -(int) ( point.x()-calibration.x())/TRIESTE_MOTORSTEPSIZE;
   int nstepsY = -(int) ( point.y()-calibration.y())/TRIESTE_MOTORSTEPSIZE;
   int nstepsZ = 2*((int) ( point.z()-calibration.z())/TRIESTE_MOTORSTEPSIZE);

   string buffer =
      string("F,C,")+
      string("S2M")+stringify<int>(objectspeed)+
      string(",S3M")+stringify<int>(objectspeed)+
      string(",S4M")+stringify<int>(objectspeed)+
      string(",IA2M")+stringify<int>(nstepsX)+
      string(",IA3M")+stringify<int>(nstepsY)+
      string(",IA4M")+stringify<int>(nstepsZ)+
      string(",R");

   sendCommand(5,buffer);
}

void moveObject(const Eigen::Vector3d &delta, int objectspeed)
{  int nstepsX = -(int) ( delta.x())/TRIESTE_MOTORSTEPSIZE;
   int nstepsY = -(int) ( delta.y())/TRIESTE_MOTORSTEPSIZE;
   int nstepsZ = 2*((int) ( delta.z())/TRIESTE_MOTORSTEPSIZE);

   nstepsX==0 ? nstepsX=1 : nstepsX=nstepsX;
   nstepsY==0 ? nstepsY=1 : nstepsY=nstepsY;
   nstepsZ==0 ? nstepsZ=1 : nstepsZ=nstepsZ;

   string buffer =
      string("F,C,")+
      string("S2M")+stringify<int>(objectspeed)+
      string(",S3M")+stringify<int>(objectspeed)+
      string(",S4M")+stringify<int>(objectspeed)+
      string(",I2M")+stringify<int>(nstepsX)+
      string(",I4M")+stringify<int>(nstepsZ)+
      string(",I3M")+stringify<int>(nstepsY)+
      string(",R");

   sendCommand(5,buffer);
}

void moveMirror(double deltaz, double speed)
{  int nstepsZ = (int) ( deltaz )/TRIESTE_MOTORSTEPSIZE;
   string buffer =
      string("F,C,")+
      string("S1M")+stringify<int>(speed)+
      string(",I1M")+stringify<int>(nstepsZ)+
      string(",R");
   sendCommand(5,buffer);
}

void moveMirrorAbsolute(double finalFocalDistance, double baseFocalDistance, int speed)
{  int nstepsX =  (int) ((baseFocalDistance- finalFocalDistance)/TRIESTE_MOTORSTEPSIZE);
   if ( nstepsX==0)
      return;
   string buffer =
      string("F,C,")+
      string("S1M")+stringify<int>(speed)+
      string(",IA1M")+stringify<int>(nstepsX)+
      string(",R");

   sendCommand(5,buffer);
}

void moveMirrorAbsoluteAsynchronous(double finalFocalDistance, double baseFocalDistance, int speed)
{  boost::thread motorthread1(boost::bind(&moveMirrorAbsolute,finalFocalDistance,baseFocalDistance,speed));
}

void moveObjectAbsoluteAsynchronous(const Eigen::Vector3d &point, const Eigen::Vector3d &calibration, int objectspeed)
{  boost::thread motorthread1(boost::bind(&moveObjectAbsolute,point,calibration,objectspeed));
}

void moveMonitorAbsolute(double focalDistance, double homeFocalDistance, int Monitorspeed)
{  int nstepsX = (int)(focalDistance-homeFocalDistance)/(TRIESTE_MOTORSTEPSIZE);
   if ( nstepsX==0)
      return;
   string buffer =
      string("F,C,")+
      string("S1M")+stringify<int>(Monitorspeed)+
      string(",IA1M")+stringify<int>(nstepsX)+
      string(",R");

   sendCommand(3,buffer);
}

void moveMonitorAbsoluteAsynchronous(double focalDistance, double homeFocalDistance, int Monitorspeed)
{  boost::thread motorthread1(boost::bind(&moveMonitorAbsolute,focalDistance,homeFocalDistance,Monitorspeed));
}

}
