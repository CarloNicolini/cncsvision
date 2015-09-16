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

#include <vector>
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "RigidBody.h"

using namespace std;
using namespace Eigen;

/**
*  \ingroup Geometry
*  Constructor
*  Create a new RigidBody, centroid vector, traslation vector and number of points are set to zero
*
**/
RigidBody::RigidBody()
{
    fullTransformation = Affine3d::Identity();
}

/**
*  \ingroup Geometry
*  \brief Copy constructor
*  @param  other Const reference to the other rigidbody we want to copy
*
**/
RigidBody::RigidBody(const RigidBody &other)
{
   // a copy of points stored
   points = other.points;
   pointMatrix = other.pointMatrix;
   fullTransformation.matrix() = other.fullTransformation.matrix();
}

/**
*  \ingroup Geometry
*  \brief Destructor, clean all the stored points
*
**/
RigidBody::~RigidBody()
{

   points.clear();

}

/**
*  \ingroup Geometry
*  \brief Set this object to have the points given as input points
*  \param p1 First point
*  \param p2 Second point
*  \param p3 Third point
**/

void RigidBody::setRigidBody( const Vector3d &p1,const Vector3d &p2,const Vector3d &p3)
{  points.resize(3);
   points.at(0)=pointMatrix.col(0)=p1;
   points.at(1)=pointMatrix.col(1)=p2;
   points.at(2)=pointMatrix.col(2)=p3;

}

/**
*  \ingroup Geometry
*  \brief Print this rigid body points and centroid (for debug purposes)
**/
void RigidBody::print()
{
   cerr << fullTransformation.matrix() << endl;
}

/**
*  \ingroup Geometry
*  Here we provide the simplest, fastest and most accurate method to compute the rigid body transformation between two
*  sets of points, aka the absolute orientation problem with the Umeyama algorithm.
*
*
*  The transformation representation is \f$ [\mathbf{R},\mathbf{t} ]\f$
*  where \f$\mathbf{R}\f$ is an ordinary rotation 3x3 matrix and \f$ \mathbf{t} \f$ is a 3x1 column-vector
*  Reference:
*  S. Umeyama,
*  "Least squares estimation of transformation parameters between two point patterns"
*  IEEE Transactions on pattern analysis and machine intelligence, 13,4 (1991)
*  Another reference is
*  D.W Eggert, A. Lorusso, R.B. Fisher
*  "Estimating rigid body transformation: a comparison of four major algorithms"
*  Machine Vision and applications, 1994
*
*  @param otherRigidBody The other rigidbody for which we want to align the points
*  @param isScaling True if a scaling factor between the two sets of points is needed, false otherwise.
*  @return true if the transformation is computed successfull, false otherwise
*
**/
bool RigidBody::computeTransformation(const RigidBody &otherRigidBody , bool isScaling )
{  fullTransformation.matrix() = Eigen::umeyama( pointMatrix, otherRigidBody.pointMatrix , isScaling);
   return true;
}

/**
*  \ingroup Geometry
*  Return a const reference to the full (4x4) affine transformation
*  @return Eigen::Affine3d
*
**/
const Affine3d& RigidBody::getFullTransformation() const
{  return fullTransformation;
}

/**
*  \ingroup Geometry
*  Return a Vector3d representing the
*  @return Eigen::Vector3d
*
**/
const Vector3d RigidBody::getAngleAxis(double &angle) const
{  AngleAxis<double> aa(fullTransformation.rotation());
   angle = aa.angle();

   return Vector3d( aa.axis().x(), aa.axis().y() , aa.axis().z());
}

/**
*  \ingroup Geometry
*  Return the pitch angle in radians of the current transformation (rotation about x axis)
*  @return double
**/
const double RigidBody::getPitch() const
{
	return 	fullTransformation.linear().eulerAngles(1,0,2).y();
}

/**
*  \ingroup Geometry
*  Return the yaw angle in radians of the current transformation (rotation about y axis)
*  @return double
**/
const double RigidBody::getYaw() const
{
	return 	fullTransformation.linear().eulerAngles(1,0,2).x();
}

/**
*  \ingroup Geometry
*  Return the roll angle in radians of the current transformation (rotation about z axis)
*  @return double
**/
const double RigidBody::getRoll() const
{
	return 	fullTransformation.linear().eulerAngles(1,0,2).z();
}
