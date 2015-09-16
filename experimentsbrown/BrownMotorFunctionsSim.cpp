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

#include "BrownMotorFunctionsSim.h"

/**
* \ingroup Communication
* Home object synchronously (blocking call)
*
* \param objectspeed Object speed
**/
namespace BrownMotorFunctionsSim
{
void homeObject(int objectspeed)
{  
cerr << "test" << endl;
/* SerialStream serial("COM3");

   // Home object on Z axis
   string buffer("F,C,S"+stringify<int>(BROWN_OBJECT_Z_AXIS)+"M"+stringify<int>(objectspeed)+
                 ",S"+stringify<int>(BROWN_OBJECT_Y_AXIS)+"M"+stringify<int>(objectspeed)+
                 ",I"+stringify<int>(BROWN_OBJECT_Z_AXIS)+"M0"+
                 ",I"+stringify<int>(BROWN_OBJECT_Y_AXIS)+"M0"+
                 ",IA"+stringify<int>(BROWN_OBJECT_Z_AXIS)+"M-0"+
                 ",IA"+stringify<int>(BROWN_OBJECT_Y_AXIS)+"M-0"+
                 ",R");

   cerr << "Sending:" << buffer << endl;
   serial << buffer << endl;
   char out=0;
   serial >> out;
   if ( out=='^' )
      cerr << "OK" << endl;
   else
      cerr << "Error in homeObject()" << endl;
*/
}
/**
* \ingroup Communication
* Home object asynchronously
*
* \param objectspeed Object speed
**/
void homeObjectAsynchronous(int objectspeed)
{  
//boost::thread motorthread1(boost::bind(&homeObject,objectspeed));
}

/**
* \ingroup Communication
* Home screen synchronously (blocking call)
*
* \param screenspeed Screen speed
**/
void homeScreen(int screenspeed)
{ /* SerialStream serial("COM3");

   // Home object on Z axis
   string buffer("F,C,S"+stringify<int>(BROWN_SCREEN_X_AXIS)+"M"+stringify<int>(screenspeed)+
                 ",I"+stringify<int>(BROWN_SCREEN_X_AXIS)+"M0"+
                 ",IA"+stringify<int>(BROWN_OBJECT_Z_AXIS)+"M-0"+
                 ",R");

   cerr << "Sending:" << buffer << endl;
   serial << buffer << endl;

   char out=0;
   serial >> out;
   if ( out=='^' )
      cerr << "OK" << endl;
   else
      cerr << "Error in homeScreen()" << endl;
*/
}

/**
* \ingroup Communication
* Home both screen and object synchronously (blocking call)
*
* \param objectspeed Object speed
* \param screenspeed Screen speed
**/
void homeEverything(int objectspeed, int screenspeed)
{ 
	cerr << "Object goes home." << endl;
	cerr << "Screen goes home." << endl;
}

/**
* \ingroup Communication
* Home both screen and object asynchronously
*
* \param objectspeed Object speed
* \param screenspeed Screen speed
**/
void homeEverythingAsynchronous(int objectspeed, int screenspeed)
{  //boost::thread motorthread1(boost::bind(&homeEverything,objectspeed,screenspeed));
}

/**
* \ingroup Communication
* Move the Object at absolute position given a Optotrak coordinate system (calibration) synchronously (blocking call)
*
* \param point Final absolute point to reach
* \param calibration Current calibration of Optotrak coordinate system
* \param objectspeed Object speed
**/
void moveObjectAbsolute(const Eigen::Vector3d &point, const Eigen::Vector3d &calibration, int objectspeed)
{ 

	int nstepsY = -(point.y()-calibration.y())/BROWN_MOTORSTEPSIZE;
	int nstepsZ =  (point.z()-calibration.z())/BROWN_MOTORSTEPSIZE;

	cerr << "Object is brought to " << point.y() << " Y and " << point.z() << " Z" << endl;
}

/**
* \ingroup Communication
* Move the Screen at absolute focal distance given its home focal distance w.r.t a given calibration synchronously (blocking call)
*
* \param focalDistance Final focal distance
* \param homeFocalDistance Focal distance at home given the current calibration of Optotrak coordinate system
* \param screenspeed Screen speed (should be less than 4000 steps/sec)
**/
void moveScreenAbsolute(double focalDistance, double homeFocalDistance, int screenspeed)
{ 
	int nstepsX = (int)(focalDistance-homeFocalDistance)/(BROWN_MOTORSTEPSIZE);
	cerr << "Fixation is brought to " << focalDistance << " Z" << endl;
}

/**
* \ingroup Communication
* Move the Screen at absolute focal distance given its home focal distance w.r.t a given calibration asynchronously (non blocking call)
*
* \param focalDistance Final focal distance
* \param homeFocalDistance Focal distance at home given the current calibration of Optotrak coordinate system
* \param screenspeed Screen speed (should be less than 4000 steps/sec)
**/
void moveScreenAbsoluteAsynchronous(double focalDistance, double homeFocalDistance, int screenspeed)
{  //boost::thread motorthread1(boost::bind(&moveScreenAbsolute,focalDistance,homeFocalDistance,screenspeed));
}

/**
* \ingroup Communication
*  Read the steps value for the 3 axis motors: X=screen motor, Y=object y, Z=object z
*  \return Positions in steps (integers)
**/
Eigen::Vector3i readPositions()
{  /*SerialStream serial("COM3");

   // Home object on Z axis
   string Z,X,Y;
   serial << "Z" << endl;
   serial >> Z;

   serial << "X" << endl;
   serial >> X;

   serial << "Y" << endl;
   serial >> Y;
   // girati cosi per questo particolare sistema X,Y,Z corrispondono a dire assi 1,2,3
   double x = str2num<int>(Z)*BROWN_MOTORSTEPSIZE;
   double y = str2num<int>(X)*BROWN_MOTORSTEPSIZE;
   double z = str2num<int>(Y)*BROWN_MOTORSTEPSIZE;

   Eigen::Vector3i positionIntegers( str2num<int>(Z),str2num<int>(X),str2num<int>(Y) );
   Eigen::Vector3d position(x,y,z);
   cerr << "Absolute position" << position.transpose() << "[mm]" << endl;
   return positionIntegers;
*/
}

/**
* \ingroup Communication
* Home the Screen on X axis asynchronously
*
* \param speed Screen speed
**/
void homeScreenAsynchronous(int speed)
{ // boost::thread motorthread1(boost::bind(&homeScreen,speed));
}

/**
* \ingroup Communication
* Home the Object on Z,Y axis asynchronously
*
* \param speed Object speed
**/
void homeObjectAsynchronouse(int speed)
{ // boost::thread motorthread1(boost::bind(&homeObject,speed));
}


/**
* \ingroup Communication
* Move the Object on Z,Y axis synchronously
*
* \param delta Object displacement
* \param speed Object speed
**/
void moveObject(const Eigen::Vector3d &delta, int speed)
{  
	if ( delta.norm() > 1e20 )
	{  std::cerr << "Impossible position to reach! Check marker visibility!" << endl;
		return;
	}

	Eigen::Vector3d displacement(0,-delta.y(),delta.z());
	cerr << "Object moves " << -delta.y() << " vertically." << endl;
	cerr << "Object moves " << delta.z() << " in depth." << endl;
}

/**
* \ingroup Communication
* Move the Object on Z,Y axis synchronously
*
* \param from Object start position
* \param to Object end position
* \param speed Object speed
**/
void moveObject(const Eigen::Vector3d &from, const Eigen::Vector3d &to, int speed)
{ // speed > 6000 ? speed = 6000 : speed = speed;
   moveObject(to-from, speed);
}

/**
* \ingroup Communication
* Move the Screen on X axis synchronously
*
* \param delta Screen displacement
* \param speed Screen speed
**/
void moveScreen(const Eigen::Vector3d &delta, int speed)
{ /* if ( delta.norm() > 1e20 )
   {  std::cerr << "Impossible position to reach! Check marker visibility!" << endl;
      return;
   }
   VmxLinearMotor screenMotor;
   speed > 3500 ? speed = 3500 : speed = speed;
   screenMotor.moveSteps(BROWN_MOTORS_COM_PORT, BROWN_SCREEN_X_AXIS ,delta.x(),3000,BROWN_MOTORSTEPSIZE);
*/
}

/**
* \ingroup Communication
* Move the Screen on X axis synchronously
*
* \param from Screen start position
* \param to Screen end position
* \param speed Screen speed
**/
void moveScreen(const Eigen::Vector3d &from, const Eigen::Vector3d &to, int speed)
{ // moveScreen(from-to, speed);
}


/**
* \ingroup Communication
* Move the object on Z,Y axis synchronously
*
* \param delta Object displacement
* \param speed Object speed
**/
void moveObjectAsynchronous(const Eigen::Vector3d &delta, int speed)
{ // boost::thread motorthread1(boost::bind(&moveObject,delta,speed));
}

/**
* \ingroup Communication
* Move the object on Z,Y axis totally asynchronously
*
* \param from Object start position
* \param to Object end position
* \param speed Object speed
**/
void moveObjectAsynchronous(const Eigen::Vector3d &from, const Eigen::Vector3d &to, int speed)
{ // Eigen::Vector3d delta = to-from;
  // boost::thread motorthread1(boost::bind(&moveObject,delta,speed));
}

/**
* \ingroup Communication
* Move the screen on X axis totally asynchronously
*
* \param delta Screen displacement
* \param speed Screen speed
**/
void moveScreenAsynchronous(const Eigen::Vector3d &delta, int speed)
{ // boost::thread motorthread1(boost::bind(&moveScreen,delta,speed));
}

/**
* \ingroup Communication
* Move the screen on X axis totally asynchronously
*
* \param from Screen starting position
* \param to Screen end position
* \param speed Screen speed
**/
void moveScreenAsynchronous(const Eigen::Vector3d &from, const Eigen::Vector3d &to, int speed)
{
/*
   Eigen::Vector3d delta = to-from;
   boost::thread motorthread1(boost::bind(&moveScreen,delta,speed));
*/
}

/**
* \ingroup Communication
* Move the object on Z axis and the screen on X axis together, then move the object on Y axis synchronously (blocking call)
*
* \param objectDelta Object displacement
* \param screenDeltaScreen displacement
* \param objectspeed Object speed
* \param screenspeed Screen speed (optional otherwise set to 3500)
**/
void moveObjectAndScreen(const Eigen::Vector3d &objectDelta, const Eigen::Vector3d &screenDelta, int objectspeed, int screenspeed )
{ /* if ( objectDelta.norm() < BROWN_MOTORSTEPSIZE )
      return;

   // Prepare the serial communication objects
   SerialStream serial("COM3");
   int nstepsX= (int)(screenDelta.x()/BROWN_MOTORSTEPSIZE);
   int nstepsY= (int)(-objectDelta.y()/BROWN_MOTORSTEPSIZE);
   int nstepsZ= (int)(objectDelta.z()/BROWN_MOTORSTEPSIZE);
   cerr << nstepsX << "," << nstepsY << "," << nstepsZ << endl;
   if ( nstepsX==0 || nstepsZ==0 )
      return;

   // Move screen on X axis and object on Z axis together
   string buffer("F,C,(S"+stringify<int>(BROWN_OBJECT_Z_AXIS)+"M"+stringify<int>(objectspeed)+
                 ",S"+stringify<int>(BROWN_SCREEN_X_AXIS)+"M"+stringify<int>(screenspeed)+
                 ",I"+stringify<int>(BROWN_SCREEN_X_AXIS)+"M"+stringify<int>(nstepsX)+
                 ",I"+stringify<int>(BROWN_OBJECT_Z_AXIS)+"M"+stringify<int>(nstepsZ));
   // command termination
   buffer+=",)R";
   cerr << "Sending:" << buffer << endl;
   serial << buffer << endl;
   char out=0;
   serial >> out;
   if ( out=='^' )
      cerr << "OK" << endl;
   else
      cerr << "Error in moveObjectAndScreen()" << endl;

   if (nstepsY==0)
      return;
   buffer=string("F,C,S")+stringify<int>(BROWN_OBJECT_Y_AXIS)+string("M")+stringify<int>(objectspeed)+
          string(",I")+stringify<int>(BROWN_OBJECT_Y_AXIS)+string("M")+stringify<int>(nstepsY)+string(",R");
   cerr << "Sending:" << buffer << endl;
   serial << buffer << endl;
   out=0;
   serial >> out;
   if ( out=='^' )
      cerr << "OK" << endl;
   else
      cerr << "Error in moveObjectAndScreen()" << endl;
*/
}

/**
* \ingroup Communication
* Move the object on Z axis and the screen on X axis together, then move the object on Y axis synchronously (blocking call)
*
* \param objectFrom Object initial position
* \param objectTo Object final desidered position
* \param screenFrom Screen initial position
* \param screenTo Screen final position
* \param objectspeed Object speed
* \param screenspeed Screen speed
**/
void moveObjectAndScreen(const Eigen::Vector3d &objectFrom,const Eigen::Vector3d &objectTo,const Eigen::Vector3d &screenFrom,const Eigen::Vector3d &screenTo,int objectspeed,int screenspeed )
{ // moveObjectAndScreen(objectTo-objectFrom,screenTo-screenFrom,objectspeed,screenspeed);
}

/**
* \ingroup Communication
* Move the object on Z axis and the screen on X axis together, then move the object on Y axis totally asynchronously
*
* \param objectDelta Object displacement
* \param screenDelta Screen displacement
* \param objectspeed Object speed
* \param screenspeed Screen speed
**/
void moveObjectAndScreenAsynchronous(const Eigen::Vector3d &objectDelta,const Eigen::Vector3d &screenDelta, int objectspeed,int screenspeed )
{ // boost::thread motorthread1(boost::bind(&moveObjectAndScreen,objectDelta,screenDelta,objectspeed,screenspeed));
}

/**
* \ingroup Communication
* Move the object on Z axis and the screen on X axis together, then move the object on Y axis totally asynchronously
* \param objectFrom Object initial position
* \param objectTo Object final desidered position
* \param screenFrom Screen initial position
* \param screenTo Screen final position
* \param objectspeed Object speed
* \param screenspeed Screen speed
**/
void moveObjectAndScreenAsynchronous(const Eigen::Vector3d &objectFrom, const Eigen::Vector3d &objectTo,const Eigen::Vector3d &screenFrom, const Eigen::Vector3d &screenTo, int objectspeed, int screenspeed )
{ // boost::thread motorthread1(boost::bind(&moveObjectAndScreen,objectTo-objectFrom,screenTo-screenFrom,objectspeed,screenspeed));
}

}
