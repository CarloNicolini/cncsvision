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

// OpenGL Camera Class
//
// Author: Stephen Manley - smanley@nyx.net
// Date: Oct 12th 1999
//
// Description:
//
// This is the decription for the camera class
//
//
// This class also take some method from the Avogadro Camera class
//

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <Eigen/Core>
#include <Eigen/Geometry>
#include "IncludeGL.h"
#define ORTHOGRAPHIC = 10000;
#define PROJECTION   = 10001;
using namespace Eigen;

#define LEFTEYE -1
#define RIGHTEYE 1

/**
* \class Camera
* \ingroup GLVisualization
* The simple camera class is useful to do simple opengl setups and command the positions of the eye via keyboars.
*
**/

class Camera
{
public:
   EIGEN_MAKE_ALIGNED_OPERATOR_NEW
   Camera();
   ~Camera();

   // Actions
   void lookAtScene(double iod );
   inline void calculateNormal();
   void reset();
   void print();
   // Data matrices and vector
   Vector3d normal;
   Vector3d view;
   Vector3d rotationEye;
   Vector3d eye;
   Vector3d center;
   Vector3d up;
   // modelview matrix as a affine transformation
   Affine3d modelview;
   // Projection matrix
   Matrix4d projection;

   // Motion
   void moveX(double _x);
   void moveY(double _y);
   void moveZ(double _z);

   void moveEye(double dx, double dy, double dz);
   void moveCenter(double dx, double dy, double dz);

   void rotateEye(double dx, double dy);

   // Scaling
   void scaleXYZ(double a,  double b, double c);

   // Avogadro camera part
   void normalize();
   void initializeViewPoint();
   void translate( const Vector3d &);
   void pretranslate( const Vector3d &tras );
   void rotate( const double &angle, const Vector3d &axis);
   void prerotate(const double &angle, const Vector3d &axis);
   double distance(const Vector3d & point) const ;
   void setModelview(const Transform<double,3,Affine> &matrix);


   void applyPerspective() const;
   void applyModelview() const;
   Vector3d backTransformedXAxis() const;
   Vector3d backTransformedYAxis() const;
   Vector3d backTransformedZAxis() const;
   Vector3d transformedXAxis() const;
   Vector3d transformedYAxis() const;
   Vector3d transformedZAxis() const;
   Vector3d unProject(const Vector3d& v, int, int) const;
   Vector3d project(const Vector3d & v, int width, int height)  const;

};

#endif /*_CAMERA_H_*/
