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

Vector3d getReflected(const Vector3d &p, const Hyperplane<double,3> &plane)
{
	Vector3d n  = plane.normal();
	Vector3d r = p- (2*p.dot(n)-plane.offset() )/(n.dot(n) )*n;
	return r;
}

int main(int argc, char *argv[])
{  Vector3d p1(-1,0,1);
   Vector3d p2(1,0,1);
   Vector3d p3(1,1,1);
   Vector3d t(0,0,-10);

   Hyperplane<double,3> h = Hyperplane<double,3>::Through(p1,p2,p3);
   cout << "NORMAL PLANE= " << h.normal().transpose() << endl;
   cout << "OFFSET PLANE= " << h.offset() << endl;

   vector<Vector3d> points,reflectedPoints;
   for (int i=0; i<100; i++)
   points.push_back(Vector3d::Random());
   
    for (int i=0; i<100; i++)
   reflectedPoints.push_back(getReflected(points.at(i), h) );
   
   for (int i=0; i<100; i++)
   	cout << points[i].transpose() << endl;
   
   for (int i=0; i<100; i++)
   	cout << reflectedPoints[i].transpose() << endl;
   
   return 0;
}


