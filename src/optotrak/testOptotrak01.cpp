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

/*
Description:
  Optotrak Certus Sample

  - Initiate communications with the Optotrak System.
  - Determine SCU version.
  - Determine strober configuration.
  - Set number of markers to activate on each strober.
  - Retrieve/display device properties.
  - Set processing flags to perform data conversions on the host computer.
  - Load the appropriate camera parameters.
  - Set up an Optotrak collection.
  - Activate the markers.
  - Request/receive/display 5 frames of real-time 3D data.
  - De-activate the markers.
  - Stop the Optotrak collection.
  - Disconnect from the Optotrak System.
*/

#include <iostream>
#include <string>

#include "Point3D.h"
#include "RigidBody.h"
#include "Optotrak.h"

char cameraFile[] = "C:/cncsvisiondata/cameraFiles/LatestCameraAlignment.cam";
using namespace std;

Timer timer;

int main(int argc, char*argv[])
{  Optotrak optotrak;
   int numMarker=20;
   optotrak.init(numMarker,cameraFile,NULL,"optotrak.log");

   timer.start();
   while (timer.getElapsedTimeInMilliSec()<5000)
   {
       optotrak.updatePoints();
       vector<Vector3d> points = optotrak.getAllPoints();
       for (int i=0; i<points.size();i++)
           cout << points[i].transpose() << endl;
       cout << "=====" << endl;
   }

   optotrak.stopCollection();

   return 0;
}

