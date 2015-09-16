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
#include "BrownMotorFunctions.h"


using namespace std;
using namespace Eigen;
using namespace boost::posix_time;
using namespace BrownMotorFunctions;

int main(int argc, char* argv[])
{

   homeObject(4000);
   homeObjectAsynchronous(5000);
   homeScreen(3500);
   homeScreenAsynchronous(3500);
   homeEverything(3000,3000);
   homeEverythingAsynchronous(3000,3000);
   moveObjectAbsolute(Vector3d(0,1,2), Vector3d(0,1,2), 2000);
   moveScreenAbsolute(-418.5,-518.5,3500);
   moveScreenAbsoluteAsynchronous(-418.5,-518.5,3500);
   moveObject(Vector3d(1,2,3), 2500);
   moveObjectAsynchronous(Vector3d(1,2,3), 3500);
   moveObject(Vector3d(0,0,0), Vector3d(0,1,0), 3500);
   moveObjectAsynchronous(Vector3d(0,0,0), Vector3d(0,1,0), 3500);
   moveScreen(Vector3d(1,2,3), 3500);
   moveScreenAsynchronous(Vector3d(1,2,3), 3500);

   moveScreen(Vector3d(0,0,0), Vector3d(0,1,0), 3500);
   moveScreenAsynchronous(Vector3d(0,0,0), Vector3d(0,1,0), 3500);
   moveObjectAndScreen(Vector3d(0,3,0), Vector3d(0,3,0), 3500, 3500 );
   moveObjectAndScreenAsynchronous(Vector3d(0,3,0),Vector3d(0,3,0), 3500,3500 );

   moveObjectAndScreen(Vector3d(0,1,0),Vector3d(0,2,0),Vector3d(0,2,0),Vector3d(0,2,0),3500,3500 );
   moveObjectAndScreenAsynchronous(Vector3d(0,1,0), Vector3d(0,2,0),Vector3d(0,2,0), Vector3d(0,2,0),3500, 3500 );
   homeObject(3500);
   homeObjectAsynchronous(3500);
   homeScreen(3500);
   homeEverything(3500, 3500);
   homeEverythingAsynchronous(3500, 3500);
   moveObjectAbsolute(Vector3d(1,2,3), Vector3d(1,2,3), 3500);
   moveScreenAbsolute(-418.5,-568.5, 3500);
   moveScreenAbsoluteAsynchronous(-418.5,-568.5, 3500);
   homeScreenAsynchronous(3500);
   homeObjectAsynchronouse(3500);
   moveObject(Vector3d(1,2,3), 3500);
   moveObject(Vector3d(0,0,0), Vector3d(0,1,0), 3500);
   moveScreen(Vector3d(0,1,0), 3500);
   moveScreen(Vector3d(0,0,0), Vector3d(0,1,0), 3500);
   moveObjectAsynchronous(Vector3d(0,1,0), 3500);
   moveObjectAsynchronous(Vector3d(0,0,0), Vector3d(0,1,0), 3500);
   moveScreenAsynchronous(Vector3d(0,1,0), 3500);
   moveScreenAsynchronous(Vector3d(0,0,0), Vector3d(0,1,0), 3500);
   moveObjectAndScreen(Vector3d(0,3,0), Vector3d(0,1,0), 3500, 3500 );
   moveObjectAndScreen(Vector3d(0,1,0),Vector3d(0,2,0),Vector3d(0,1,0),Vector3d(0,1,0),3500,3500 );
   moveObjectAndScreenAsynchronous(Vector3d(0,3,0),Vector3d(0,1,0), 3500,3500 );
   moveObjectAndScreenAsynchronous(Vector3d(0,1,0), Vector3d(0,2,0),Vector3d(0,1,0), Vector3d(0,1,0), 3500, 3500 );


   return 0;
}

