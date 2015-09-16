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

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "EulerExtractor.h"
#include "Mathcommon.h"

using namespace std;
using namespace Eigen;
using namespace mathcommon;

EulerExtractor::EulerExtractor()
{


}
/**
* \ingroup Geometry
* Initialize a EulerExtractor object starting from a \f$ 3\times 3 \f$ rotation matrix
**/
void EulerExtractor::init(const Matrix3d & R)
{  angles = R.eulerAngles(1,0,2);       // here we adopt the yaw pitch roll convention
   yaw = angles.x();
   pitch = angles.y();
   roll = angles.z();

   //cout << toDegrees(yaw2) << " " << toDegrees(pitch2) << " " << toDegrees(roll2) << endl;
   //Quaternion<double> q(R);
   //init(q);
}

/**
* \ingroup Geometry
* Initialize a EulerExtractor object starting from a quaternion.
**/
void EulerExtractor::init(Quaternion<double> &q)
{  double qx=q.x();
   double qy=q.y();
   double qz=q.z();
   double qw=q.w();

   yaw = atan2(2.0*qy*qw - 2.0*qx*qz, 1-2.0*qy*qy-2.0*qz*qz );
   pitch = asin(2.0*qx*qy + 2.0*qz*qw);
   roll = atan2(2.0*qx*qw - 2.0*qy*qz, 1-2.0*qx*qx-2.0*qz*qz );

}

/**
* \ingroup Geometry
* Extract roll (rotation angle around x axis) in radians
* \return roll
**/
double EulerExtractor::getRoll()
{  return roll;
}

/**
* \ingroup Geometry
* Extract yaw (rotation angle around y axis) in radians
* \return yaw
**/
double EulerExtractor::getYaw()
{  return yaw;
}

/**
* \ingroup Geometry
* Extract pitch (rotation angle around z axis) in radians
* \return pitch
**/
double EulerExtractor::getPitch()
{  return pitch;
}

/**
* Print to standard error all the Euler angles in radians
*
**/
void EulerExtractor::print()
{  cerr << "Yaw(around y)= " << toDegrees(yaw) << " " << "\tPitch(around x)= " << toDegrees(pitch) << " " << "\tRoll(around z)= " << toDegrees(roll) << endl;
}


