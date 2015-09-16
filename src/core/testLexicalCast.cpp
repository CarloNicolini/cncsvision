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
using namespace util;

int main(int argc, char *argv[])
{  // Convert vector of string separated by commas to vector of integers
   string x(" 1,2,3,4 ");
   cout <<  str2num<int>(x,",") << endl;

   // convert double to string
   double p=1.12345678910;
   //cout << scientific << stringify<double>(p) << endl;

   vector<double> xx = vlist_of<double>(1.4)(2.3)(3.2)(4.1);
   cout << util::stringify< std::vector<double> >(xx) << endl;

   Vector3d v = Vector3d::Random();
   cout << stringify< Matrix<double,1,3> >(v) << endl;
   return 0;
}
