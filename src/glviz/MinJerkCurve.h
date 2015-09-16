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

#ifndef _MINJERK_H_
#define _MINJERK_H_

#include "IncludeGL.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif
#include <Eigen/Core>

/**
* \class MinJerkCurve
* \ingroup GLVisualization
* \brief This class is intended to compute points along a minimum jerk trajectory given start and end points of it and
* other parameters.
*
* The parameters are described in Smeeths and Brenner and readapted by us.
* Here we refer to a motion in the full-volume but with the following conventions:
* \f$\hat{x}\f$ is the right of the subject, \f$ \hat{y}\f$ is the up direction and \f$ \hat{z} \f$ points toward the subject, so that the subject looks ahead toward
* the negative z-axis.
*
* Here we use these equations to compute the interpolating points, given \f$ t\in [0,1] \f$,
* \f$ \mathbf{r}_0,\mathbf{r}_1 \f$ respectively the start and end points of the trajectory,
* \f$ \mathbf{A} \f$ the so called approach parameter, \f$ \mathbf{R} \f$ the radius of the sphere (or of the cylinder) we
* want to reach, we have :
*
* 3D Sphere :
*
* \f{eqnarray*}{
* x(t) &= x_0 + t^3 \left( \frac{1}{2} a \cos \theta \sin \phi (t-1)^2 + (x_1 +r\cos \theta \sin \phi - x_0   ) J(t) \right)  \\
* y(t) &= y_0 + t^3 \left( \frac{1}{2} a \cos \phi (t-1)^2 + (y_1 +r\cos \phi - y_0   ) J(t) \right ) \\
* z(t) &= z_0 + t^3 \left( \frac{1}{2} a \sin \theta \sin \phi (t-1)^2 + (z_1 +r\sin \theta \sin \phi - z_0   ) J(t) \right)
* \f}
*
*
* 3D Cylinder :
* \f{eqnarray*}{
* x(t) & x_0 + t^3 \left( \frac{1}{2} a \cos \phi (t-1)^2 + (x_1 +r\cos \phi - x_0   ) J(t) \right) \\
* y(t) &= y_0 + t^3 \left( (y_1 - y_0) J(t) \right) \\
* z(t) &= z_0 + t^3 \left( \frac{1}{2} a \sin \phi (t-1)^2 + (z_1 +r\sin \phi - z_0   ) J(t) \right) \\
* \f}
*
* 3D Sphere (vectorial form)
* \f[
* \mathbf{r}(t)=\mathbf{r}_0 + t^3 \big \left ( \frac{1}{2}\mathbf{A}_S(t-1)^2 +(\mathbf{r}_1 - \mathbf{r}_0 + \mathbf{R}_S) J(t) \big \right)
* \f]
* Where :
* \f{eqnarray*}{
* \mathbf{A}_S &= ( a \cos \theta \sin \phi, a \sin \phi \sin \theta, a\cos \theta )  \\
* \mathbf{R}_S &= (r \cos \theta \sin \phi, r \sin \phi \sin \theta, r\cos \theta )
* \f}
*
* 3D Cylinder :
* \f[
* \mathbf{r}(t)=\mathbf{r}_0 + t^3 \big \left ( \frac{1}{2}\mathbf{A}_S(t-1)^2 +(\mathbf{r}_1 - \mathbf{r}_0 + \mathbf{R}_C) J(t) \big \right)
* \f]
*
* Where :
*
* \f{eqnarray*}{
* \mathbf{A}_C &= ( a \cos \theta ,0, a\sin \theta ) \nonumber \\
* \mathbf{R}_C &= (r \cos \theta , 0, r\sin \theta )  \nonumber
* \f}
*
**/

class MinJerkCurve
{
public:
   MinJerkCurve();
   ~MinJerkCurve();

   void init( unsigned int _npoints, int mode, const Eigen::Vector3d &_start, const Eigen::Vector3d &_endKnot  );
   void draw();

   Eigen::Vector3d interpolate( double t );
   Eigen::Vector3d interpolateFinger( double t );
   Eigen::Vector3d interpolateThumb( double t );

   void drawInterpolatedFinger(double t);
   //void drawInterpolatedThumb(double t); XXX to deprecate


protected:
   int mode;
   unsigned int numPoints;
   Eigen::Vector3d start, end;
   double phi,theta;
   double cosphi;
   double sinphi;
   double costheta;
   double sintheta;

   double approachParameter;
   double distance;
   double radius;

   GLUquadric *quad;

   static const int sphereMode = 0;
   static const int cylinderMode = 1;
};

#endif
