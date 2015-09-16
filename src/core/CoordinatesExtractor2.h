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

#ifndef _CoordinatesExtractor2_H
#define _CoordinatesExtractor2_H

#include <vector>
#include <Eigen/Core>

#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include "RigidBody.h"
#include "Marker.h"
#include "Timer.h"
#include "DaisyFilter.h"


using namespace std;
using namespace Eigen;

/**
*
* \class CoordinatesExtractor2
* \ingroup Geometry
* \brief The CoordinatesExtractor2 class is an elegant way to define rigid body starting from 3 points.
* The approach followed is: first take a shot of all the points to learn the relative distances and transformations, then continously update the patch
* points and get the computed left and right eye coordinates.
* In detail :
*
* Let's say that we take a shot of your head dressing the patch with the 3 points and the glasses with the two points on nodal eye distances.
* We identify the three points on the patch with \f$ \mathbf{p}_1, \mathbf{p}_2, \mathbf{p}_3 \f$ and the two points on the glasses with
* \f$ \mathbf{m}_l, \mathbf{m}_r \f$ and the eye points with \f$ \mathbf{e}_l, \mathbf{e}_r \f$.
* Starting from the three patch points \f$ \mathbf{p}_1, \mathbf{p}_2, \mathbf{p}_3 \f$ we are able to define a plane passing through them using the
* \code Eigen::Hyperplane<double,3,1> \endcode class. The set of patch points is then univoquely defined with its centroid \f$ \mathbf{c}=\frac{ \mathbf{p}_1 +  \mathbf{p}_2 + \mathbf{p}_3 }{3} \f$ and its normal unit vector \f$ \mathbf{\hat{n}} \f$.
* After the first call of \code CoordinatesExtractor2::init() \endcode we save the relative distances from the centroid of patch points and the markers on the glasses, then at every call of the
* \code CoordinatesExtractor2::update() \endcode we compute the relative transformation that maps the points on the patch \f$ \mathbf{p}_1, \mathbf{p}_2, \mathbf{p}_3 \f$ at $t=0$
* with the new points \f$ \mathbf{p}_1(t), \mathbf{p}_2(t), \mathbf{p}_3(t) \f$.
* We call this transformation \f$ \mathbf{g} = (\mathbf{R}, \mathbf{T} ) \f$ where \f$ \mathbf{R }\f$ is a \f$ 3 \times \f$ rotation matrix and \f$ \mathbf{T} \f$ is a translation vector.
* The computation is done via the \ref RigidBody class.
* First we calculate the new mask markers positions using the relative inverse affine transformation \f$ \mathbf{g}^{-1} \f$ extracted with \code RigidBody::getFullTransformation().inverse() \endcode
* then to estimate the eye positions \f$ \mathbf{p}_{el}, \mathbf{p}_{er} \f$ simply use this formula given the IOD.
\f[
\mathbf{p}_{el} =  \frac{ \mathbf{m}_l + \mathbf{m}_r}{2} - \frac{ \mathbf{m}_r - \mathbf{m}_l }{ || \mathbf{m}_r - \mathbf{m}_l || }\frac{IOD}{2}
\f]
* That's all!
* This version output Marker instead of simple points, so you can couple it directly with the Optotrak2 or Optotrak3 classes which outputs Marker with their velocity and accelerations too.
**/

class CoordinatesExtractor2
{
public:
   // This is for a pointers alignment issue, read the following website for further informations:
   // http://eigen.tuxfamily.org/dox/StructHavingEigenMembers.html
   EIGEN_MAKE_ALIGNED_OPERATOR_NEW

   CoordinatesExtractor2();
   ~CoordinatesExtractor2();
   void init(const Vector3d &leftMask, const Vector3d &rightMask,
             const Vector3d &p1, const Vector3d &p2, const Vector3d &p3, double iod=60 );
   void init(const Vector3d &fingerStart,const Vector3d &p1, const Vector3d &p2, const Vector3d &p3 );
   void init(const vector<Marker> &markers, const Vector3d &p1, const Vector3d &p2, const Vector3d &p3 );

   void setInterOcularDistance(double iod);
   void setFilterVelocity(bool, double gain);
   void update(const Vector3d &p1, const Vector3d &p2, const Vector3d &p3, double deltaTInMilliSeconds );
   void update(const Marker &p1, const Marker &p2, const Marker &p3, double deltaTInMilliSeconds );

   const Marker &getLeftEye(); // to be used in VRCamera as left eye
   const Marker &getRightEye();// to be used in VRCamera as right
   const Marker &getCyclopeanEye(); // to be used in VRCamera as central cyclopean Eye
   const Marker &getP1();
   const Marker &getP2();

   const Matrix3d getRotationMatrix() const;
   const Vector3d getRotationAxis() const;
   const double getYaw() const;
   const double getPitch() const;
   const double getRoll() const;

   // General method
   const vector<Marker> &getTransformedMarkers();


   RigidBody& getRigidStart();
private:
   vector < vector < DaisyFilter* > > filters;
   double filterGain;
   bool initialized;
   bool filterVelocity;
   bool headMode;
   vector <Marker> startPoints;
   vector <Marker> currentPoints, prevpoints,prevprevpoints;

   void clear();
   void clearFilter();
   void initFilter(int size, double gain);
   // Only for eye position computations
   vector<Marker> eyeLeft;
   vector<Marker> eyeRight;
   vector<Marker> cyclopeanEye;

   double interOcularDistance;

   RigidBody rigidStart,rigidEnd;    // the rigidbody of the points on the patch at three time frames

};

#endif
