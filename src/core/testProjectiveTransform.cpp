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
#include <vector>
#include <cmath>
#include <deque>
#include <algorithm>
#include <iomanip>

#include <Eigen/Core>
#include <Eigen/QR>
#include <Eigen/LU>
#include <Eigen/Geometry>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include "Mathcommon.h"
#include "RigidBody.h"
#include "EulerExtractor.h"
#include "Timer.h"
#include "CoordinatesExtractor.h"
#include "Screen.h"
#include "MatrixStream.h"
#include "Util.h"

using namespace std;
using namespace mathcommon;
using namespace Eigen;

#define NPOINTS 1E2



void randomizeSeed(void)
{  int stime;
   long ltime;
   ltime = time(NULL);
   stime = (unsigned) ltime / 2;
   srand(stime);
}


int main(int argc, char *argv[])
{

   // Test all the point3D functionalities
   randomizeSeed();
   Projective3d pippo2;

   cout << util::str2num<double>(std::string("192.192")) << endl;

   return 0;
}
