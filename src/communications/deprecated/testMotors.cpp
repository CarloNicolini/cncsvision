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
#include <stdexcept>
#include <Eigen/Core>
#include <boost/thread.hpp>
#include "SerialStream.h"
#include "VmxLinearMotor.h"
#include "VmxRotationMotor.h"
#include "Timer.h"
using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace Eigen;

int main(int argc, char* argv[])
{
	/*
   Vector3d start(-192, 272.5, -390.4);
   Vector3d delta(0.5,0.5,0.5);
   Vector3d end = start + delta;
   VmxLinearMotor motor;

   try
   {  motor.move3d(5, start, end, 1000);
   }
   catch ( std::exception const& e )
   {  cerr << e.what() << endl;
   }

   // motor.move3d(3, 1000, start, end);
   // motor.move3d(4, 1000, start, end);
   */
   VmxRotationMotor rotMotor;
   int speed=2000;
   Timer timer;
   while ( speed != 0)
   {
	   cerr << "insert speed: " << endl;
	   cin >> speed;
	   timer.start();
	   rotMotor.rotate(30.0,speed);
	   cerr << timer.getElapsedTimeInMilliSec() << endl;
   }
   return 0;
}

