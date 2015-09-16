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

#include <cmath>
#include <iostream>
#include <fstream>
#include "Mathcommon.h"

#include "Camera.h"

#define CAMERA_NEAR_DISTANCE 1.0

using namespace std;
using namespace Eigen;
using namespace mathcommon;

Camera::Camera()
{  // Initial eye position
   eye << 0, 0, -20;

   // Initial scene center
   center.setZero(3);

   // Initial up normal vector
   up = Vector3d::UnitY();
   rotationEye.setZero(3);

   calculateNormal();
}

Camera::~Camera() {}

void Camera::reset()
{
// Initial eye position
   eye << 0, 0, -20;

   // Initial scene center
   center.setZero(3);
   // Initial up normal vector
   up = Vector3d::UnitY();
   rotationEye.setZero(3);

   calculateNormal();
}

void Camera::scaleXYZ(double a, double b, double c)
{

}


///////////////////////
// NON EIGEN PART
//////////////////////

// Rotates about the eye - changes the scene center
void Camera::rotateEye(double dx, double dy)
{

   Vector3d newScene, newUp, newScene1;
   rotationEye.x()+=dx;
   rotationEye.y()+=dy;

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glRotated(dy, normal.x(), normal.y(), normal.z() );
   glGetDoublev(GL_MODELVIEW_MATRIX, (double*)modelview.matrix().data() );

   newUp = modelview*up;
   up = newUp;
   newScene = modelview*view;

   glLoadIdentity();
   glRotated(dx, up.x(), up.y(), up.z());
   glGetDoublev(GL_MODELVIEW_MATRIX, (double*)modelview.matrix().data());
   glPopMatrix();

   newScene1 = modelview*newScene;

   center = eye+newScene1;
   calculateNormal();
}


void Camera::print()
{  glGetDoublev(GL_MODELVIEW_MATRIX, (double*)modelview.matrix().data());
   glGetDoublev( GL_PROJECTION_MATRIX, (double*)projection.data() );

   cout << "MODELVIEW MATRIX\n" << modelview.matrix() << endl;
   cout << "PROJECTION MATRIX\n" << projection << endl;
}

void Camera::calculateNormal()
{  // Determine viewing vector
   view = center - eye;
   // Calculate the cross product of the view and up vectors
   normal = view.cross(up);
   // Set to be a unit normal
   normal.normalize();

}

// Left and right are along the normal vector to the camera - normals
// only change under a rotation.
void Camera::moveX(double dist)
{  eye     += normal*(dist);
   center += normal*(dist);
   calculateNormal();
}

// Up and down are along the camera's up and down vector
void Camera::moveY(double dist)
{  eye    += up*(dist);
   center += up*(dist);
   calculateNormal();
}

void Camera::lookAtScene(double halfInterOcularDist )
{  gluLookAt( eye.x()  ,eye.y(),eye.z(),
              center.x() , center.y(),center.z(),
              up.x(), up.y(), up.z());

   glGetDoublev(GL_MODELVIEW_MATRIX, (double*)modelview.matrix().data() );
}

// Forward and back are along the viewing vector
void Camera::moveZ(double dist)
{  eye += ( view*(dist));
   center += view*(dist);
   calculateNormal();
}


void Camera::moveEye(double dx, double dy, double dz)
{  eye+=Vector3d(dx,dy,dz);
   calculateNormal();
}

void Camera::moveCenter(double dx, double dy, double dz)
{  center+=Vector3d(dx,dy,dz);
   calculateNormal();
}
/*

// EIGEN PART
void Camera::normalize()
{  Block<Matrix4d, 3, 1> c0(modelview.matrix(), 0, 0),
   c1(modelview.matrix(), 0, 1),
   c2(modelview.matrix(), 0, 2);
   c0.normalize();
   c1.normalize();
   c1 -= c0.dot(c1) * c0;
   c1.normalize();
   c2.normalize();
   c2 -= c0.dot(c2) * c0;
   c2 -= c1.dot(c2) * c1;
   c2.normalize();
   modelview.matrix().row(3) << 0, 0, 0, 1;
}

void Camera::translate( const Vector3d &tras )
{  modelview.translate(tras);
}

void Camera::pretranslate( const Vector3d &tras )
{  modelview.pretranslate(tras);
}

void Camera::rotate( const double &angle, const Vector3d &axis)
{  modelview.rotate(AngleAxisd(angle, axis));
   normalize();
}

void Camera::prerotate(const double &angle, const Vector3d &axis)
{  modelview.prerotate(AngleAxisd(angle, axis));
   normalize();
}

double Camera::distance(const Vector3d & point) const
{  return ( modelview * point ).norm();
}

void Camera::setModelview(const Transform<double,3,Affine> &matrix)
{  modelview = matrix;
}

Vector3d Camera::backTransformedXAxis() const
{  return modelview.linear().row(0).transpose();
}

Vector3d Camera::backTransformedYAxis() const
{  return modelview.linear().row(1).transpose();
}

Vector3d Camera::backTransformedZAxis() const
{  return modelview.linear().row(2).transpose();
}

Vector3d Camera::transformedXAxis() const
{  return modelview.linear().col(0);
}

Vector3d Camera::transformedYAxis() const
{  return modelview.linear().col(1);
}

Vector3d Camera::transformedZAxis() const
{  return modelview.linear().col(2);
}

void Camera::initializeViewPoint()
{  modelview.setIdentity();
// we want a top-down view on it, i.e. the scene should fit as well as
// possible in the (X,Y)-plane. Equivalently, we want the Z axis to be parallel
// to the normal vector of the molecule's fitting plane.
// Thus we construct a suitable base-change rotation.
   Matrix3d rotation;
   rotation.row(2) = Vector3d::UnitZ(); // XXX L'ho messo io
   rotation.row(0) = rotation.row(2).unitOrthogonal();
   rotation.row(1) = rotation.row(2).cross(rotation.row(0));

// set the camera's matrix to be (the 4x4 version of) this rotation.
   modelview.linear() = rotation;

// now we want to move backwards, in order
// to view the molecule from a distance, not from inside it.
// This translation must be applied after the above rotation, so we
// want a left-multiplication here. Whence pretranslate().
   const Vector3d Zaxis(0,0,1);
   double radius=1.0;
   pretranslate( - 3.0 * ( radius + CAMERA_NEAR_DISTANCE ) * Zaxis );

}

void Camera::applyModelview() const
{  glMultMatrixd( modelview.data() );
}
*/

