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

#ifndef _EULER_EXTRACTOR_
#define _EULER_EXTRACTOR_
#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace Eigen;

/**
* \class EulerExtractor
* \ingroup Geometry
* \brief The EulerExtractor class is a simple wrapper around the Eigen::Matrix3d class used as a rotation matrix.
*
* The main purpose of this class is to extract 3 euler angles from a generic rotation matrix following the aircraft convention.
* We follow the convention given in http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
* which is also called the yxz convention
* and we use the following formulae in order to extract yaw, pitch and roll
* First we construct a quaternion from the rotation matrix via the given Eigen's Quaternion<double>::Quaternion<double>( const Matrix3d &R)
* then we compute the Euler angles with the following formulas
* For the y-axis rotation, we obtain (in radians) the yaw (or heading or azimuth or \f$ \theta \f$ )
*
* \f[
* yaw = \arctan2 ( 2 q_y q_w - 2q_x q_z, 1-2q_y^2 - 2q_z^2 )
* \f]
*
**
* For the pitch ( or attitude or elevation or \f$ \phi \f$ ):
*
* \f[
* \mathrm{pitch} = \arcsin ( 2 q_x q_y + 2q_zq_w )
* \f]
*
* For the roll ( or bank or tilt or \f$ \psi \f$ ):
* \f[
* roll = \arctan2 ( 2 q_x q_w - 2q_yq_z, 1-2q_x^2-2q_z^2 )
* \f]
*
* Where
* \f[
* \mathbf{q} \in \mathcal{R}^4, \mathbf{q} = (q_w, q_x, q_y, q_z )
* \f]
* is a unit quaternion as computed from the rotation matrix.
**/

class EulerExtractor
{
public:
   EIGEN_MAKE_ALIGNED_OPERATOR_NEW
   EulerExtractor();
   void init(const Matrix3d & );
   void init(Quaternion<double> &q );

   double getRoll();
   double getYaw();
   double getPitch();
   void print();

private:
   Vector3d angles;

   double yaw, pitch, roll;

};
#endif
