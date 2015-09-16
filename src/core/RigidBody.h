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

#ifndef _RIGID_BODY
#define _RIGID_BODY

#include <vector>
#include <iostream>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>

#include "Mathcommon.h"

using namespace std;
using namespace Eigen;
using namespace mathcommon;

/**
* \class RigidBody
* \ingroup Geometry
* \brief The RigidBody class is an abstraction of a physical rigid body. A phyisical rigid body is an object for which positions of its constituting points are fixed one to each other.
* RigidBody can be used in smart calibration of reference frames. During the program execution time, points can be added, removed, reassigned.
*
*
* RigidBody class offers the possibility of calibration of reference frames.
* Suppose you have a set of points \f$ \mathbf{d}_i \f$ in a global coordinates frame called \f$ \mathcal{D} \f$ and the same set of points in another
* coordinates frame \f$ \mathcal{M} \f$, and you want to find which transformation of coordinates maps the points \f$ \mathbf{d}_i \f$ to the points \f$ \mathbf{m}_i \f$.
* This problem is also called in literature as the "Orthogonal procrustes problem".
* Given two sets of points, \f$ {\mathbf{d}_i}\f$  and \f${m_i}\f$ with \f$ i=1,...,N \f$ we want to find the transformation \f$ A=[\mathbf{R},\mathbf{t}]  \f$ such that
*
* \f[ \mathbf{d}_i = \mathbf{R} \mathbf{m}_i + \mathbf{t} + \mathbf{N}_i \f]
* where \f$ \mathbf{R} \f$ is a 3x3 rotation matrix, \f$ \mathbf{t} \f$ is a 3D traslation vector.

* Solving for the optimal transformation \f$ [\tilde{\mathbf{R}},\tilde{\mathbf{T}}] \f$ requires a least-squares error criterion minimization
*
* \f[
* O^2 = \sum \limits_{i=1}^N || \mathbf{d}_i -\mathbf{R} \mathbf{m}_i - \mathbf{t} ||^2
* \f]

* Calculating 3x3 rotation matrix is done via SVD. We can show that:
* \f[
* \mathbf{R} = \mathbf{V} \mathbf{U}^T
* \f]
* where the 3x3 matrices \f$ \mathbf{V} \f$ and \f$ \mathbf{U} \f$ are the singular value decomposition of \f$ \mathbf{H} = \sum_i^N \left< \mathbf{ m }_i \right > \left < \mathbf{d}_i \right > ^T \f$
* \f[ H = \mathbf{U} \mathbf{\Sigma} \mathbf{V}^T \f]
* and \f$ \left< \mathbf{ m }_i \right >, \left< \mathbf{ d }_i \right >\f$ are the centroid distance for each of the two set of points.
* The traslation \f$ t \in \mathcal{R}^3 \f$ is computed as
* \f[
* \mathbf{\tilde{\mathbf{t}}} = \left < \mathbf{d}_i \right > - \tilde{\mathbf{R}} \left< \mathbf{ m }_i \right >
* \f]
*
*
* Example code:
* \code
*     vector<Point3D> vPointStart(5);  // The starting set
*  for (int i=0; i<5; i++) // We fill with uniform random values the starting set
*     vPointStart.at(i) = Point3D( unifRand(10)%10, unifRand(10)%10 ,0  );
*
*  vector<Point3D> vPointEnd(5);    // The arrival set
*  Point3D t(1,2,3);    // The relative traslation
*  for (int i=0; i<5; i++)    // We first traslate then rotate the arrival set
*  {
*     vPointEnd.at(i) = Point3D( (vPointStart.at(i)) );
*     vPointEnd.at(i)+=t;
*     vPointEnd.at(i).rotateZ( M_PI/2.0 );
*
*  }
*  \endcode
*
* Now that we have our two vectors, we fill the two rigid bodies with them:
* \code
*  RigidBody rigidBodyStart, rigidBodyEnd;
*  rigidBodyStart.setRigidBody( vPointStart );
*  rigidBodyEnd.setRigidBody( vPointEnd );
*  // Now we do the internal computations, this set the internal rotation matrix and traslation of rigidBodyStart
*  rigidBodyStart.computeTransformation(rigidBodyEnd);
* \endcode
* We can access the transformed points with the getTransformedPoints() method that gives us a reference to the vector of transformedPoints
* vector<Point3D> X = rigidBodyStart.getTransformedPoints();
* In order to check the correctness of the implementation ( Giuda's approach ), try the following:
* \code
* vector<Point3D> trial = R1.getTransformedPoints();
*  for (int i=0; i< trial.size(); i++)
*     cerr << (trial[i] - vPointEnd[i]).length() << endl;
* \endcode
* Usually this will print a very little value of norm of difference vector
*
* Further informations can be found at the following article:
*  D.W.Eggert, A.Lorusso, R.B.Fisher:
*  Estimating 3D rigid body transformation: A comparison of four major algorithms
*  Machine vision and applications (1997)
*
**/

class RigidBody
{
public:
    // http://eigen.tuxfamily.org/dox/StructHavingEigenMembers.html added for a C++98 issue
   EIGEN_MAKE_ALIGNED_OPERATOR_NEW
   RigidBody();
   RigidBody(const RigidBody&);
   ~RigidBody();
   void setRigidBody( const Vector3d &,const Vector3d &,const Vector3d &);
   void print();
   bool computeTransformation( const RigidBody & , bool isScaling =false);
   const Affine3d& getFullTransformation() const;
   const Matrix3d &getPointMatrix() const;

   const double getRoll() const;
   const double getPitch() const;
   const double getYaw() const ;
   const Vector3d getAngleAxis(double &) const;

private:
   std::vector<Vector3d> points;
   Matrix3d pointMatrix;
   Affine3d fullTransformation;
};

#endif
