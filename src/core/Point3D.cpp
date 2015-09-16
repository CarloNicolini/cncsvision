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
#include <Eigen/QR>
#include <Eigen/Geometry>
#include <cmath>
#include <complex>
#include <stdexcept>

#include "Point3D.h"

using namespace std;
using namespace Eigen;

/**
* \ingroup Geometry
* Standard constructor
**/
#ifdef INLINECONSTRUCTORS
Point3D::Point3D() : x(0.0),y(0.0),z(0.0), radius(0.4)
{
}

/**
* \ingroup Geometry
* Initialized constructor
**/
Point3D::Point3D(double _x, double _y, double _z) : x(_x), y(_y),z(_z),radius(0.4)
{
}
/**
* \ingroup Geometry
* copy constructor
**/
Point3D::Point3D(const Point3D &other) : x(other.x),y(other.y),z(other.z),radius(other.radius)
{
}
/**
* \ingroup Geometry
* Constructor from Eigen::Vector3d
**/
Point3D::Point3D(const Vector3d &v) : x(v.x()),y(v.y()),z(v.z()),radius(0.4)
{
}

/**
* \ingroup Geometry
*  Destructor
**/
Point3D::~Point3D()
{

}

#endif
/**
* \ingroup Geometry
* Vector by scalar multiplication
* @param a Scalar scale factor
* \f[\mathbf{y}=a \mathbf{x}  \f]
**/
Point3D Point3D::operator*(double a) const
{  return Point3D(x * a, y * a, z * a);
}

/**
* \ingroup Geometry
* Get column vector obtained from column-vector matrix multiplication
* \f[    (\mathbf{y},1)=\mathbf{A}(\mathbf{x},1) \f]
* Where:
* \f[ \mathbf{A} \in \mathcal{R}^{3 \times 3} \f]
* @param m The 3x3 matrix
* @return the scaled point
*
**/
Point3D Point3D::operator*(const Matrix3d &m) const
{
	if ( (m.rows()!=3) || (m.cols()!=3) )
		throw std::runtime_error("Inconsistent multiplication!");
   Vector3d vec(x,y,z);// column vector
   Vector3d vec2 = m*vec;
   double cx=vec2[0];
   double cy=vec2[1];
   double cz=vec2[2];

   return Point3D(cx,cy,cz);
}

/**
* \ingroup Geometry
* Get column vector obtained from affine transformation. The 4th column is [0,0,0,1]^T
* @param m The 4x4 Affine3d matrix
* See Eigen::Geometry module for details
* We extend 3D vectors adding an 1 as fourth dimension
* \f$ \mathbf{x}_4 = (\mathbf{x},1) \f$ and \f$ \mathbf{y}_4 = (\mathbf{y},1) \f$
* \f[    \mathbf{y}_4=\mathbf{A} \mathbf{x}_4 \f]
* Where:
* \f[ \mathbf{A} \in \mathcal{R}^{4 \times 4} \f]
* and we return the first 3 elements of \f$ \mathbf{y} \f$
* This is the classical methods to include incorporate affine rotations in a 4D matrix
**/
Point3D Point3D::operator*(const Affine3d &aff) const
{  Vector3d tmp(aff*Vector3d(x,y,z));
   return Point3D(tmp.x(),tmp.y(),tmp.z());
}

/**
* \ingroup Geometry
* Vector by scalar division
* @param a Scalar scale factor
* \f[\mathbf{y}=\mathbf{x}/a  \f]
* @return divided point
**/
Point3D Point3D::operator/(double scale) const
{  return Point3D(x / scale, y / scale, z / scale);
}

/**
* \ingroup Geometry
* Vector sum
* @param other vector to sum
* \f[\mathbf{a}=\mathbf{b} + \mathbf{c}   \f]
* @return summed point
**/
Point3D Point3D::operator+(const Point3D &other) const
{  return Point3D(x + other.x, y + other.y, z + other.z);
}

/**
* \ingroup Geometry
* Vector sum
* \f$ v \in \mathcal{R}
* @param v double to sum to each component
* \f[\mathbf{a}=\mathbf{b} + (v,v,v)   \f]
* @return a point with all the components summed to v
**/
Point3D Point3D::operator+(const double &v) const
{  return Point3D(x +v , y +v , z + v);
}

/**
* \ingroup Geometry
* Vector subtraction
* \f[\mathbf{a}=\mathbf{b} - \mathbf{c}   \f]
* @return a point with all the components summed to v
**/
Point3D Point3D::operator-(const Point3D &other) const
{  return Point3D(x - other.x, y - other.y, z - other.z);
}

/**
* \ingroup Geometry
* Vector cross-product
*
* \f[\mathbf{a}=\mathbf{b} \times \mathbf{c} \f]
* Usage example:
* \code
* Point3D b(1,0,0),c(0,1,0);
* Point3D a= b^c;
* \endcode
* @return cross-product between this and other
**/
Point3D Point3D::operator^(const Point3D &other) const
{  return (*this).cross(other);
}
/// get opposite vector
Point3D Point3D::operator-() const
{  return Point3D(-x, -y, -z);
}

Point3D Point3D::operator-(double scalar) const
{  return Point3D(x-scalar, y-scalar,z-scalar);
}

double Point3D::operator[](unsigned int i) const
{  switch (i)
   {  case 0:
         return x;
      case 1:
         return y;
      case 2:
         return z;
   }
   return x;
}

double& Point3D::operator[](unsigned int i)
{  switch (i)
   {  case 0:
         return x;
      case 1:
         return y;
      case 2:
         return z;
   }
   return x;
}

/// Scalar multiplication
const Point3D &Point3D::operator*=(double scale)
{  x *= scale;
   y *= scale;
   z *= scale;
   return *this;
}
/**
* \ingroup Geometry
* Matrix multiplication
* @param m Eigen::Matrix3d matrix type
**/
void Point3D::operator*=(const Matrix3d &m)
{  if ( (m.rows()!=3) || (m.cols()!=3) )
	throw std::runtime_error("Inconsistent multiplication!");
   
   Vector3d vec(x,y,z);
   Vector3d vec2 = m*vec;

   x=vec2[0];
   y=vec2[1];
   z=vec2[2];
}

void Point3D::operator*=(const Affine3d &aff)
{  // XXX Should be corrected, there is an unuseful overheading of temporary resources
   Vector3d tmp(aff*Vector3d(x,y,z));
   x=tmp.x();
   y=tmp.y();
   z=tmp.z();
}

/// Vector by scalar division
const Point3D &Point3D::operator/=(double scale)
{  x /= scale;
   y /= scale;
   z /= scale;
   return *this;
}

/// Vector sum
const Point3D &Point3D::operator+=(const Point3D &other)
{  x += other.x;
   y += other.y;
   z += other.z;
   return *this;
}
/// Vector subtraction
const Point3D &Point3D::operator-=(const Point3D &other)
{  x -= other.x;
   y -= other.y;
   z -= other.z;
   return *this;
}
void Point3D::set(double _x, double _y, double _z)
{  x=_x;
   y=_y;
   z=_z;
}
/// Get dot-product between this and other
double Point3D::dot(const Point3D &other)
{  return ((x * other.x) +  (y * other.y) + (z * other.z));
}

/**
* \ingroup Geometry
* \brief Reflect the point w.r.t. a specified plane ax+by+cz = 0
*
* @param a x coefficient of the plane
* @param b y coefficient of the plane
* @param c z oefficient of the plane
**/
void Point3D::reflect(double a, double b, double c, double d)
{  reflect(Vector3d(a,b,c));
}

/**
* \ingroup Geometry
* \brief Reflect the point w.r.t. a specified plane ax+by+cz = 0
*
* @param _v the coefficient of the plane in the order a,b,c
**/
void Point3D::reflect(const Vector3d &_v)
{  Vector3d v=_v.normalized();
   double dot= x*v.x()+y*v.y()+z*v.z();
   x -= 2.0*dot*v.x();
   y -= 2.0*dot*v.y();
   z -= 2.0*dot*v.z();
}

/**
* \ingroup Geometry
* \brief Reflect the point w.r.t. a specified plane ax+by+cz = 0
*
* @param a x coefficient of the plane
* @param b y coefficient of the plane
* @param c z oefficient of the plane
* @return The reflected point
**/
Point3D Point3D::getReflected(double a, double b, double c, double d)
{  Matrix3d P = Matrix3d::Identity();
   Vector3d v(a,b,c);
   v.normalize();
   P -= 2*v*v.transpose();
   return Point3D(x,y,z)*P;
}

/**
* \ingroup Geometry
* \brief Reflect the point w.r.t. a specified plane ax+by+cz = 0
*
* @param _v the coefficient of the plane in the order a,b,c
* @return The reflected point
**/
Point3D Point3D::getReflected(const Vector3d &_v)
{  Matrix3d P = Matrix3d::Identity();
   Vector3d v=_v;
   v.normalize();
   P -= 2*v*v.transpose();
   return Point3D(x,y,z)*P;

}

/// Get cross-product between this and other
Point3D Point3D::cross(const Point3D &other) const
{  return Point3D(y * other.z - z * other.y,
                  z * other.x - x * other.z,
                  x * other.y - y * other.x);
}
/// Scalar multiplication
Point3D operator*(double scale, const Point3D &v)
{  return v * scale;
}

/// Compare vector by pointer
bool Point3D::operator!=(const Point3D &rhs) const
{  return !( this == &rhs );
}

/// Compare vector by pointer
bool Point3D::operator==(const Point3D &rhs) const
{  return ( this == &rhs );
}
/// Cross product in place operator
void Point3D::operator^=(const Point3D &other)
{  double _x = y * other.z - z * other.y;
   double _y = z * other.x - x * other.z;
   double _z = x * other.y - y * other.x;
   x=_x;
   y=_y;
   z=_z;

}
/// Print vector to output stream
ostream &operator<<(ostream &output, const Point3D &v)
{  output << v.x << ", " << v.y << ", " << v.z ;
   return output;
}


/// get this vector norm
double Point3D::length() const
{  return sqrt(x * x + y * y + z * z);
}

/// get this vector 2-norm
double Point3D::length2() const
{  return x * x + y * y + z * z;
}

/// get distance from this to other
double Point3D::distance(Point3D &other) const
{  return sqrt(pow(x-other.x,2) + pow(y-other.y,2) + pow(z-other.z,2));
}
/// Normalize this vector
void Point3D::normalize()
{  double m = sqrt(x * x + y * y + z * z);
   x/=m;
   y/=m;
   z/=m;
}

/**
* \ingroup Geometry
* L2-normalized vector
* @return L2-normalized vector
**/
Point3D Point3D::normalized() const
{  double invM = 1./sqrt(x * x + y * y + z * z);

   return Point3D(x*invM,y*invM,z*invM);
}
/**
* \ingroup Geometry
* Linear interpolation
* Given two vector v0 (*this)  and v1, find a parametrized line that interpolates them 0 < factor < 1
**/
Point3D Point3D::lerp(const Point3D &r, double factor) const
{  return (*this) + ( r - (*this) ) * factor;
}
/**
* \ingroup Geometry
* Spherical linear interpolation
* Given two vector v0 (*this)  and v1, find a parametrized curve that interpolates them along a sphere 0 < factor < 1
*
**/
Point3D Point3D::slerp(const Point3D &v1, double factor) const
{

   Point3D p0 = (*this).normalized();
   Point3D p1 = v1.normalized();

   Point3D rotAxis = (p0.cross(p1)).normalized();
   double thetaZero = acos(p0.dot(p1));
   double theta = factor*thetaZero;

   Point3D returnPoint = (*this).normalized();
   returnPoint.rotate(rotAxis,theta);

   return returnPoint;

}


/// Compute 1/vector
void Point3D::invert()
{  x=1./x;
   y=1./y;
   z=1./z;
}

/// Compute inverted vector
Point3D Point3D::inverse() const
{  return Point3D(1.0/x,1.0/y,1.0/z);
}

/** Rotate vector of angle around axis X
 @param angle (in radians, positive=clockwise, negative=counterclockwise)
**/
void Point3D::rotateX(double angle)
{  Matrix3d Rx = Matrix3d::Identity();

   double c = cos(angle);
   double s = sin(angle);


   Rx(1,1)=c ;
   Rx(1,2)= -s ;
   Rx(2,1)=s ;
   Rx(2,2)= c;

   (*this)*=Rx;
}

/**
* \ingroup Geometry
* Rotate vector of angle around axis Y
* @param angle (in radians, positive=clockwise, negative=counterclockwise)
**/
void Point3D::rotateY(double angle)
{  Matrix3d Ry = Matrix3d::Identity();
   double c = cos(angle);
   double s = sin(angle);

   Ry(0,0) = c;
   Ry(0,2)= -s ;

   Ry(2,0) = s;
   Ry(2,2)= c;

   (*this)*=Ry;
}

/**
* \ingroup Geometry
* Rotate vector of angle around axis Z
* @param angle (in radians, positive=clockwise, negative=counterclockwise)
**/
void Point3D::rotateZ(double angle)
{  Matrix3d Rz = Matrix3d::Identity();
   double c = cos(angle);
   double s = sin(angle);
   Rz(0,0) = c;
   Rz(0,1) = s;
   Rz(1,0) = -s;
   Rz(1,1) = c;

   (*this)*=Rz;
}
/**
* \ingroup Geometry
* Rotate vector of angle around arbitrary given axis
* @param angle (in radians, positive=clockwise, negative=counterclockwise)
* Reference for this matrix found at:
* http://www.gamedev.net/reference/articles/article1199.asp
**/
void Point3D::rotate(const Point3D &axis, double angle)
{

   double c=cos(angle);
   double s=sin(angle);
   double t=1.0-c;
   Matrix3d R=Matrix3d::Identity();
   Point3D normAxis= axis.normalized();
   double xi = normAxis.x;
   double yi = normAxis.y;
   double zi = normAxis.z;

   R(0,0) = t*xi*xi + c;
   R(0,1)=t*xi*yi +s*zi;
   R(0,2)=t*xi*zi-s*yi ;
   R(1,0) = t*xi*yi-s*zi;
   R(1,1)= t*yi*yi+c;
   R(1,2)= t*yi*zi+s*xi ;
   R(2,0) = t*xi*zi+s*yi;
   R(2,1)=t*yi*zi-s*xi;
   R(2,2)= t*zi*zi+c;

   (*this)*=R;
}


/**
* \ingroup Geometry
* Rotate vector of angle around axis X
* @param angle (in radians, positive=clockwise, negative=counterclockwise)
*
**/
Point3D Point3D::getRotatedX(double angle)
{  Matrix3d Rx(3,3);

   double c = cos(angle);
   double s = sin(angle);

   Rx(0,0)=1.0;
   Rx(0,1)=0.0 ;
   Rx(0,2)=0.0 ;
   Rx(1,0)=0.0;
   Rx(1,1)=c ;
   Rx(1,2)= s ;
   Rx(2,0)=0.0;
   Rx(2,1)=-s ;
   Rx(2,2)= c;

   Point3D res=(*this)*Rx;
   return res;
}

/**
* \ingroup Geometry
* Rotate vector of angle around axis Y
* @param angle (in radians, positive=clockwise, negative=counterclockwise)
**/
Point3D Point3D::getRotatedY(double angle)
{  Matrix3d Ry(3,3);
   double c = cos(angle);
   double s = sin(angle);

   Ry(0,0) = c;
   Ry(0,1)=0.0 ;
   Ry(0,2)= -s ;
   Ry(1,0) = 0.0;
   Ry(1,1)= 1.0 ;
   Ry(1,2)= 0.0 ;
   Ry(2,0) = s;
   Ry(2,1)=0.0;
   Ry(2,2)= c;

   Point3D res=(*this)*Ry;
   return res;
}

/**
* \ingroup Geometry
* Rotate vector of angle around axis Z
* @param angle (in radians, positive=clockwise, negative=counterclockwise)
*
**/
Point3D Point3D::getRotatedZ(double angle)
{  Matrix3d Rz(3,3);
   double c = cos(angle);
   double s = sin(angle);
   Rz(0,0) = c;
   Rz(0,1) = s;
   Rz(0,2)=0.0 ;
   Rz(1,0) = -s;
   Rz(1,1) = c;
   Rz(1,2)=0.0 ;
   Rz(2,0) = 0.0;
   Rz(2,1)=0.0;
   Rz(2,2)=1.0;

   Point3D res=(*this)*Rz;
   return res;
}
/**
* \ingroup Geometry
* Rotate vector of angle around arbitrary given axis
* @param angle (in radians, positive=clockwise, negative=counterclockwise)
* Reference for this matrix found at:
* http://www.gamedev.net/reference/articles/article1199.asp
*
**/
Point3D Point3D::getRotated(const Point3D &axis, double angle)
{

   double c=cos(angle);
   double s=sin(angle);
   double t=1.0-c;
   Matrix3d R=Matrix3d::Identity();
   Point3D normAxis= axis.normalized();
   double xi = normAxis.x;
   double yi = normAxis.y;
   double zi = normAxis.z;

   R(0,0) = t*xi*xi + c;
   R(0,1)=t*xi*yi +s*zi;
   R(0,2)=t*xi*zi-s*yi ;
   
   R(1,0) = t*xi*yi-s*zi;
   R(1,1)= t*yi*yi+c;
   R(1,2)= t*yi*zi+s*xi ;
   
   R(2,0) = t*xi*zi+s*yi;
   R(2,1)=t*yi*zi-s*xi;
   R(2,2)= t*zi*zi+c;

   Point3D res=(*this)*R;
   return res;
}


/**
* \ingroup Geometry
* Rotate this vector of angle around arbitrary given axis with a rotation pole
* @param axis (axis of rotation, no worry if unnormalized, we normalize it internally)
* @param pole (pole of rotation)
* @param angle angle expressed in radians ( positive=clockwise, negative=counterclockwise)
* Reference for this matrix found at:
* http://www.gamedev.net/reference/articles/article1199.asp
**/
void Point3D::affineRotate(const Point3D &axis, const Point3D &pole, double angle)
{  Point3D tmp((*this)-pole);
   tmp.rotate(axis,angle);
   tmp+=pole;

   x=tmp.x;
   y=tmp.y;
   z=tmp.z;
}
/**
* \ingroup Geometry
* First bring remove trasl from this, then rotate it then add trasl again
* @param R rotation matrix
* @param traslation
* @param angle angle expressed in radians ( positive=clockwise, negative=counterclockwise)
* Reference for this matrix found at:
* http://www.gamedev.net/reference/articles/article1199.asp
**/
void Point3D::rototranslate(const Matrix3d &R, const Point3D& trasl)
{  (viewFromReferenceFrame(R, trasl));
   (*this) +=trasl;

}
/** \ingroup Geometry
* \brief Return true if this point is inside a 1E12 edge length cube
* @return True if this point is inside a 1E12 edge length cube
*/
bool Point3D::isVisible()
{  bool visible;
   visible = (x > -1E12 && x < 1E12 ) && (y > -1E12 && y < 1E12 ) && (z > -1E12 && z< 1E12 );
   return visible;
}

/**
* \ingroup Geometry
* Roto-translate this vector and get a new Point3D
* @param Rot rotation matrix
* @param trasl Translation vector
* @return the point obtained from this roto-translation
* Reference for this matrix found at:
* http://www.gamedev.net/reference/articles/article1199.asp
**/
Point3D Point3D::getRototranslated( const Matrix3d& Rot, const Point3D &trasl)
{  return ( changeReferenceFrame(Rot, trasl) + trasl);
}

/**
* \ingroup Geometry
* Get the rotated of this vector of angle around arbitrary given axis with a rotation pole
* @param axis (axis of rotation, no worry if unnormalized, we normalize it internally)
* @param pole (pole of rotation)
* @param angle angle expressed in radians ( positive=clockwise, negative=counterclockwise)
* @return The point rotated around the pole and axis
* Reference for this matrix found at:
* http://www.gamedev.net/reference/articles/article1199.asp
**/
Point3D Point3D::getAffineRotated(const Point3D &axis, const Point3D &pole, double angle)
{

   Point3D tmp((*this)-pole);
   tmp.rotate(axis,angle);
   tmp+=pole;

   return tmp;
}

/**
* \ingroup Geometry
* Overloaded rotation+traslation
* Documentation at http://www.kwon3d.com/theory/transform/transform.html
*
* Beware to describe the traslation vector from the original coordinate system to the traslated one!!!
*
**/
Point3D Point3D::changeReferenceFrame(const Matrix3d &rotation, const Point3D &trasl)
{  // must include traslation (as described in eq.12 at the upper link
   Point3D local = (*this) - trasl;
   local*=rotation;

   return  local  ;
}

/**
* \ingroup Geometry
* Overloaded reference frame changing with affine transformation
* Documentation at http://www.kwon3d.com/theory/transform/transform.html
*
* Beware to describe the traslation vector from the original coordinate system to the traslated one!!!
* @return the transformed point
**/
Point3D Point3D::changeReferenceFrame(const Affine3d &full)
{  // must include traslation (as described in eq.12 at the upper link
   Point3D local = (*this)*full;

   return  local  ;
}


/**
* Overloaded rotation+traslation
* Documentation at http://www.kwon3d.com/theory/transform/transform.html
* Beware to describe the traslation vector from the original coordinate system to the traslated one!!!
**/
void Point3D::viewFromReferenceFrame(const Matrix3d &rotation, const Point3D &trasl)
{  (*this)-=trasl;
   (*this)*=rotation;

}
/**
* \ingroup Geometry
* Apply rotation and traslation to this point
* @return the transformed point
*/
Point3D Point3D::getTransformed( const Matrix3d &R, const Point3D &t )
{  return (*this)*R + t;
}

/**
* \ingroup Geometry
* Transform this point with Rotation and traslation
*
*/
void Point3D::transform( const Matrix3d &R, const Point3D &t )
{  (*this)*=R;
   (*this)+=t;
}


