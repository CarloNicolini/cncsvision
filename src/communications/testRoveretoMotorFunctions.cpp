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

//A serial class that appears as an iostream

#include <iostream>
#include <Eigen/Core>
#include "SerialStream.h"
#include "RoveretoMotorFunctions.h"


using namespace std;
using namespace Eigen;
using namespace boost::posix_time;
using namespace RoveretoMotorFunctions;

int main(int argc, char* argv[])
{  homeObject(3500);
   homeObjectAsynchronous(2500);
   homeMirror(2500);
   homeMirrorAsynchronous(2500);
   homeScreen(2500);
   homeScreenAsynchronous(2500);
   homeEverything(2500,2500,2500);
   homeEverythingAsynchronous(2500,2500,2500);
   moveObjectAbsolute(Vector3d(0,0,0), Vector3d(1,2,3), 2500);
   moveObjectAbsoluteAsynchronous(Vector3d(0,0,0), Vector3d(0,0,0), 2500);
   moveMirrorAbsolute(Vector3d(0,0,0), Vector3d(0,0,0), 2500);
   moveMirrorAbsoluteAsynchronous(Vector3d(0,0,0), Vector3d(0,0,0), 2500);
   moveScreenAbsolute(-418.5,-517.8, 2500);
   moveScreenAbsoluteAsynchronous(-418.5,-517.8, 2500);

   return 0;
}

