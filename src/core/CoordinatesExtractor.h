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

#ifndef _COORDINATESEXTRACTOR_H
#define _COORDINATESEXTRACTOR_H

#include <Eigen/Core>
#include <Eigen/Geometry>
#include "RigidBody.h"
#include "Marker.h"
#include "Timer.h"

using namespace std;
using namespace Eigen;

/**
*
* \class CoordinatesExtractor
* \ingroup Geometry
* \brief The CoordinatesExtractor class is an elegant way to define rigid body starting from 3 points.
* The approach followed is: first take a shot of all the points to learn the relative distances and transformations, then continously update the patch
* points and get the computed left and right eye coordinates.
* In detail :
*
* Let's say that we take a shot of your head dressing the patch with the 3 points and the glasses with the two points on nodal eye distances.
* We identify the three points on the patch with \f$ \mathbf{p}_1, \mathbf{p}_2, \mathbf{p}_3 \f$ and the two points on the glasses with
* \f$ \mathbf{m}_l, \mathbf{m}_r \f$ and the eye points with \f$ \mathbf{e}_l, \mathbf{e}_r \f$.
* Starting from the three patch points \f$ \mathbf{p}_1, \mathbf{p}_2, \mathbf{p}_3 \f$ we are able to define a plane passing through them using the
* \code Eigen::Hyperplane<double,3,1> \endcode class. The set of patch points is then univoquely defined with its centroid \f$ \mathbf{c}=\frac{ \mathbf{p}_1 +  \mathbf{p}_2 + \mathbf{p}_3 }{3} \f$ and its normal unit vector \f$ \mathbf{\hat{n}} \f$.
* After the first call of \code CoordinatesExtractor::init() \endcode we save the relative distances from the centroid of patch points and the markers on the glasses, then at every call of the
* \code CoordinatesExtractor::update() \endcode we compute the relative transformation that maps the points on the patch \f$ \mathbf{p}_1, \mathbf{p}_2, \mathbf{p}_3 \f$ at $t=0$
* with the new points \f$ \mathbf{p}_1(t), \mathbf{p}_2(t), \mathbf{p}_3(t) \f$.
* We call this transformation \f$ \mathbf{g} = (\mathbf{R}, \mathbf{T} ) \f$ where \f$ \mathbf{R }\f$ is a \f$ 3 \times \f$ rotation matrix and \f$ \mathbf{T} \f$ is a translation vector.
* The computation is done via the \ref RigidBody class.
* First we calculate the new mask markers positions using the relative inverse affine transformation \f$ \mathbf{g}^{-1} \f$ extracted with \code RigidBody::getFullTransformation().inverse() \endcode
* then to estimate the eye positions \f$ \mathbf{p}_{el}, \mathbf{p}_{er} \f$ simply use this formula given the IOD.
\f[
\mathbf{p}_{el} =  \frac{ \mathbf{m}_l + \mathbf{m}_r}{2} - \frac{ \mathbf{m}_r - \mathbf{m}_l }{ || \mathbf{m}_r - \mathbf{m}_l || }\frac{IOD}{2}
\f]
* That's all!
**/

class CoordinatesExtractor
{
public:
   // This is for a pointers alignment issue, read the following website for further informations:
   // http://eigen.tuxfamily.org/dox/StructHavingEigenMembers.html
   EIGEN_MAKE_ALIGNED_OPERATOR_NEW

   CoordinatesExtractor();
   void init(const Vector3d &leftMask, const Vector3d &rightMask,
             const Vector3d &p1, const Vector3d &p2, const Vector3d &p3, double iod=60 );
   void init(const Vector3d &fingerStart,const Vector3d &p1, const Vector3d &p2, const Vector3d &p3 );

   void setInterOcularDistance(double iod);
   void update(const Vector3d &p1, const Vector3d &p2, const Vector3d &p3 );
   void update(const Marker &p1, const Marker &p2, const Marker &p3 );
   void reset();

   Vector3d &getLeftEye(); // to be used in VRCamera as left eye
   Vector3d &getRightEye();// to be used in VRCamera as right
   Vector3d &getCyclopeanEye(); // to be used in VRCamera as central cyclopean Eye
   Vector3d &getFinger();
   Vector3d &getP1();
   Vector3d &getP2();

   Vector3d &getP1Vel();
   Vector3d &getP2Vel();
   Vector3d &getLeftEyeVelocity();
   Vector3d &getRightEyeVelocity();

   Vector3d& getViewingDirection(double distance);

   void setEyesPlane(const Vector3d & );

   RigidBody& getRigidStart();


private:
   Marker p1Start;     //leftMask  at t=0 or finger ( if finger mode is used )
   Marker p2Start;     //rightMask at t=0
   Marker p0Start;

   Marker p1;     //leftMask  at current time
   Marker p2;     //rightMask at current time
   Marker p0;     // a point needed to define the view direction

   // Only for eye position computations
   Marker eyeLeft;
   Marker eyeRight;
   Marker cyclopeanEye;
   Marker viewDirection;


   double interOcularDistance;
   Timer timer;

   RigidBody rigidStart[2],rigidEnd[2];    // the rigidbody of the points on the patch at three time frames

};

#endif
