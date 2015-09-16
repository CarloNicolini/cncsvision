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

#ifndef _POINT3D_H
#define _POINT3D_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include <fstream>
#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace std;
using namespace Eigen;
/**
* \defgroup Geometry
* \ingroup Geometry
* \brief The Geometry module handles everything has to do with geometric details of an experiment.
* This module relies heavily on Eigen 3.0. The most important class of the Geometry module is the \ref CoordinatesExtractor which is based on \ref RigidBody.
* Geometry is composed by the following classes in order of importance:
* <ul>
* <li> \ref CoordinatesExtractor </li>
* <li> \ref RigidBody </li>
* <li> \ref Screen </li>
* <li> \ref Point3D </li>
* <li> \ref EulerExtractor </li>
* <li> \ref MatrixStream </li>
* <li> \ref Point3D </li>
* </ul>
*
* \class Point3D
* \ingroup Geometry
* \brief Point3D is a generic base class for three-dimensional vector manipulation.
*
* It includes all the possible overloaded operators for working with vectors,
* and other geometric utilities, such as rotation around axes or around another vector
* Point3D is the base class for other objects like markers or free vectors like velocity, acceleration and so on.
* All the methods are public, this is for simplicity of the programmer, but it is supposed that you know what you're doin
* when dealing with them!
*
* Example of usage of Point3D are listed in test.cpp file, here we give some examples:
* \code
*  Point3D c0(1,0,0);
*  Point3D c1(0,1,0);
*  Point3D c2(0,0,1);
*
*  Matrix3d I3 = Matrix3d::Identity();
*
*  Vector3d v3(1,2,3);
*
*  double factor=2;
*
*  cout << "====== Here are all the methods of the class Point3D ===== " << endl;
*  cout << "Product by scalar " << c0*factor << endl;
*  cout << "Product by 3x3 matrix" << c0*I3 << endl;
*  cout << "Product by 4x4 matrix " << c0*factor << endl;
*  cout << "Division by scale " << c0/factor << endl;
*  cout << "Sum p1+p2 " << c0+c1 << endl;
*  cout << "Sum p1+p2 " << c0+c1 << endl;
*  cout << "Sum p1+scalar " << c0+factor << endl;
*  cout << "Difference p1-p2 " << c0-c1 << endl;
*  cout << "Difference p1-scalar " << c0-factor << endl;
*  cout << "Random access " << c0[0] << ", " << c0[1] << ", " << c0[2] << endl;
*  cout << "Cross product " << (c0^c1) << endl;
*
*  cout << "===== Modifiers =====" << endl;
*  c1=c0;
*  c0*=factor;
*  cout << "*= double " << c0 << endl;
*  c0=c1;
*  c0*=I3;
*  cout << "*= Matrix3d " << c0 << endl;
*  c0=c1;
*  c0*=I4;
*  c0=c1;
*  c0/=factor;
*  cout << "/= double " << c0 << endl;
*  c0=c1;
*  c0+=c1;
*  cout << "+= Point3D " << c0 << endl;
*  c0=c1;
*  c0-=c1;
*  cout << "-= Point3D "  << c0 << endl;
*  c0=c1;
*  c0=Point3D::xAxis();
*  c1=Point3D::yAxis();
*  c0^=c1;
*  cout << "^= (cross product) " << c0 << endl;
*
*  cout << "== Point3D " << (c0==c1) << endl;
*  cout << "!= Point3D " << (c0!=c1) << endl;
*  cout << "Lenght= " << c0.length() << endl;
*  cout << "Length^2= " << c0.length2() << endl;
*  cout << "Distance= " << c0.distance(c1) << endl;
*  cout << "Normalized= " << c0.normalized() << endl;
*  cout << "LinearInterp (start) " << c0.lerp(c1,0) << endl;
*  cout << "LinearInterp (end) " << c0.lerp(c1,1) << endl;
*  cout << "SphericalInterp (start) " << c0.slerp(c1,0) << endl;
*  cout << "SphericalInterp (end) " << c0.slerp(c1,1) << endl;
*  cout << "Inverse= " << c0.inverse() << endl;
*  cout << "Cross=  " << c0.cross(c2) << endl;
*  cout << "NormalizeInPlace= " << c0.normalized() << endl;
*
*  c0=Point3D::xAxis();
*  c0.rotateX(M_PI/4);
*  cout << "Rotated 45° around (1,0,0)= " << c0 << endl;
*  c0=Point3D::xAxis();
*  c0.rotateY(M_PI/4);
*  cout << "Rotated 45° around (0,1,0)= " << c0 << endl;
*  c0=Point3D::xAxis();
*  c0.rotateZ(M_PI/4);
*  cout << "Rotated 45° around (0,1,0)= " << c0 << endl;
*  c0=Point3D::xAxis();
*  c0.rotate(Point3D(1,1,1),M_PI/4.);
*  cout << "Rotated 45° around (1,1,1)= " << c0 << endl;
*  c0=Point3D::xAxis();
*  c1=Point3D::yAxis();
*  cout << "Dot x.y= " << c0.dot(c1) << endl;
* \endcode
**/

class Point3D
{

public:
// This is for a pointers alignment issue, read the following website for further informations:
// http://eigen.tuxfamily.org/dox/StructHavingEigenMembers.html
   EIGEN_MAKE_ALIGNED_OPERATOR_NEW
   // Data
   double x,y,z;
   double radius;
   // constructor
   inline Point3D() :x(0.0),y(0.0),z(0.0),radius(0.4)
   {
   };
   inline Point3D(const Point3D &other) :x(other.x),y(other.y),z(other.z),radius(other.radius)
   {
   };
   inline Point3D(double _x, double _y, double _z) :x(_x),y(_y),z(_z),radius(0.4)
   {
   };
   inline Point3D(const Vector3d &v) : x(v.x()),y(v.y()),z(v.z()),radius(0.4)
   {
   };
   // destructor
   inline ~Point3D() {};

   Point3D operator*(double scale) const ;
   Point3D operator*(const Matrix3d &m) const;
   Point3D operator*(const Affine3d &aff) const;

   Point3D operator/(double scale) const  ;

   Point3D operator+(const Point3D &other) const ;
   Point3D operator+(const double &v) const;

   Point3D operator-(const Point3D &other) const;
   Point3D operator-() const;
   Point3D operator-(double ) const;

   Point3D operator^(const Point3D &other) const;

   double operator[](unsigned int i) const;
   double& operator[](unsigned int i);

   const Point3D &operator*=(double scale) ;
   void operator*=(const Matrix3d &m);
   void operator*=(const Affine3d &aff);

   const Point3D &operator/=(double scale) ;
   const Point3D &operator+=(const Point3D &other) ;
   const Point3D &operator-=(const Point3D &other) ;
   bool operator !=(const Point3D &rhs) const;
   bool operator ==(const Point3D &rhs) const;
   void operator^=(const Point3D &other);

   void set(double, double, double);
   double length() const ;
   double length2() const ;
   double distance(Point3D &other) const;
   Point3D normalized() const;
   Point3D lerp(const Point3D &, double ) const;
   Point3D slerp(const Point3D &, double ) const;

   Point3D inverse() const;
   Point3D cross(const Point3D &other) const ;
   void normalize();

   void invert();
   void rotateX(double);
   void rotateY(double);
   void rotateZ(double);
   void rotate(const Point3D &, double);
   void affineRotate(const Point3D &, const Point3D&, double);
   void rototranslate(const Matrix3d &R, const Point3D& trasl);

   Point3D getRotatedX(double);
   Point3D getRotatedY(double);
   Point3D getRotatedZ(double);
   Point3D getRotated(const Point3D &, double);
   Point3D getAffineRotated(const Point3D &axis, const Point3D&pole, double);
   Point3D getRototranslated( const Matrix3d& Rot, const Point3D &trasl);

   double dot(const Point3D &other) ;

   void reflect(double a, double b, double c, double d);
   void reflect(const Vector3d &);
   Point3D getReflected(double a, double b, double c, double d);
   Point3D getReflected(const Vector3d &);

   Point3D changeReferenceFrame(const Matrix3d &, const Point3D &);
   Point3D changeReferenceFrame(const Affine3d &);
   void viewFromReferenceFrame(const Matrix3d &, const Point3D &);
   bool isVisible();
   void transform( const Matrix3d &R, const Point3D &t );
   Point3D getTransformed( const Matrix3d &R, const Point3D &t );

   // Eigen library adapter
   Vector3d toVec3d()
   {  return Vector3d(x,y,z);
   };

   /// Return the (1,0,0) vector corresponding to x axis
   static const Point3D xAxis()
   {  return Point3D( 1, 0, 0 );
   }
   /// Return the (0,1,0) vector corresponding to y axis
   static const Point3D yAxis()
   {  return Point3D( 0, 1, 0 );
   }
   /// Return the (0,0,1) vector corresponding to z axis
   static const Point3D zAxis()
   {  return Point3D( 0, 0, 1 );
   }
   /// Return the (0,0,0) vector corresponding to origin
   static const Point3D zero()
   {  return Point3D(0,0,0);
   }

   static const Point3D random(double a, double b )
   {  Point3D tmp;
      tmp.x = (b-a)*(double)rand()/RAND_MAX + a;
      tmp.y = (b-a)*(double)rand()/RAND_MAX + a;
      tmp.z = (b-a)*(double)rand()/RAND_MAX + a;
      return tmp;

   }

   friend ostream &operator<<(ostream &output, const Point3D &v);

};

typedef vector<Point3D*> PointsRand;
typedef vector<Point3D*>::iterator PointsRandIterator;

typedef map<unsigned int, Point3D*> Points;
typedef map<unsigned int, Point3D*>::iterator PointsIterator;

#endif
