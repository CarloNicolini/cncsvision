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

#include <iostream>
#include <fstream>
#include <string>

#include "Mathcommon.h"
#include "Screen.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace mathcommon;

/** \ingroup Geometry
* Default constructor, sets every variable to 100
**/
Screen::Screen()
{  focalDistance = screenHeight = screenWidth = 100.0;

}

/** \ingroup Geometry 
* \brief Copy constructor
**/
Screen::Screen(const Screen &other)
{
	this->focalDistance = other.focalDistance;
	this->screenHeight  = other.screenHeight;
	this->offsetX = other.offsetX;
	this->offsetY = other.offsetY;
	this->pa = other.pa;
	this->pb = other.pb;
	this->pc = other.pc;
}

/**
 * @brief Screen::Screen
 * @param _width
 * @param _height
 * @param _offsetX
 * @param _offsetY
 * @param _focalDistance
 * @param _A affine transformation to post-apply
 */
Screen::Screen(double _width, double _height, double _offsetX, double _offsetY, double _focalDistance, const Eigen::Affine3d &A)
{
    this->setWidthHeight(_width,_height);
    this->setOffset(_offsetX,_offsetY);
    this->setFocalDistance(_focalDistance);
    this->transform(A);
}

/**
 * @brief Screen::Screen
 * @param _width
 * @param _height
 * @param _offsetX
 * @param _offsetY
 * @param _focalDistance
 */
Screen::Screen(double _width, double _height, double _offsetX, double _offsetY, double _focalDistance)
{
    this->setWidthHeight(_width,_height);
    this->setOffset(_offsetX,_offsetY);
    this->setFocalDistance(_focalDistance);
}

/**
 * @brief Screen::init
 * @param _width
 * @param _height
 * @param _offsetX
 * @param _offsetY
 * @param _focalDistance
 */
void Screen::init(double _width, double _height, double _offsetX, double _offsetY, double _focalDistance)
{
    this->setWidthHeight(_width,_height);
    this->setOffset(_offsetX,_offsetY);
    this->setFocalDistance(_focalDistance);
}

/**
 * @brief Screen::init
 * @param _pa
 * @param _pb
 * @param _pc
 * @param _pd
 */
void Screen::init(const Eigen::Vector3d &_pa, const Eigen::Vector3d &_pb, const Eigen::Vector3d &_pc, const Eigen::Vector3d &_pd)
{
    pa<<_pa;
    pb<<_pb;
    pc<<_pc;
    pd<<_pd;
}

/** \ingroup Geometry
* Set the focal distance of the screen in the absolute reference frame defined (by optotrak for example)
* @param z Focal distance along the z axis (valid for a screen with a normal vector parallel to z)
**/
void Screen::setFocalDistance( double z )
{  pa.z()=pb.z()=pc.z()=pd.z()=focalDistance=z;
}

/** \ingroup Geometry
* Set the width and height of the projection screen in millimeters
* @param w Width of the monitor visible/active area
* @param h height of the monitor visible/active area
**/
void Screen::setWidthHeight(double w, double h)
{  pa << w/2.0,-h/2.0,focalDistance;
   pb << -w/2.0,-h/2.0,focalDistance;
   pc << w/2.0,h/2.0,focalDistance;
   pd << -w/2.0,h/2.0,focalDistance;

   screenWidth = w;
   screenHeight = h;
}

/** \ingroup Geometry
* Set the offset distance on XY between the z axis of global reference frame and the center of the screen
* @param x Component \f$ \hat{\mathbf{x}}}\f$ of the offset vector
* @param y Component \f$ \hat{\mathbf{y}}}\f$ of the offset vector
**/
void Screen::setOffset( double x, double y )
{  offsetX=x;
   offsetY=y;
   Eigen::Vector3d offset(x,y,0);
   pa-=offset;
   pb-=offset;
   pc-=offset;
   pd-=offset;
}

/** \ingroup Geometry
* Translate the screen
* @param x Component \f$ \hat{\mathbf{x}}}\f$ of the translation vector
* @param y Component \f$ \hat{\mathbf{y}}}\f$ of the translation vector
* @param z Component \f$ \hat{\mathbf{y}}}\f$ of the translation vector
**/
void Screen::translate(double x, double y, double z)
{  pa -= Eigen::Vector3d(x,y,z);
   pb -= Eigen::Vector3d(x,y,z);
   pc -= Eigen::Vector3d(x,y,z);
   pd -= Eigen::Vector3d(x,y,z);
   offsetX-=x;
   offsetY-=y;
   focalDistance-=z;
}

/**
 * @brief Screen::translate
 * @param t
 */
void Screen::translate(const Eigen::Vector3d &t)
{
    pa-=t;
    pb-=t;
    pc-=t;
    pd-=t;
    offsetX-=t.x();
    offsetY-=t.y();
    focalDistance-=t.z();
}

/** \ingroup Geometry
* Apply a rotation to the screen extremal points
* @param rot Rotation matrix
* One can also use a custom rotation by using the Eigen::AngleAxis<Scalar> class and the call the toRotationMatrix() method, like for example
* \code
* AngleAxis<double>aa(M_PI/3,Vector3d(0,1,0));
* screen.rotate( aa.toRotationMatrix() );
* \endcode
**/
void Screen::rotate(const Eigen::Matrix3d &rot)
{  pa = (rot*pa).eval();
   pb = (rot*pb).eval();
   pc = (rot*pc).eval();
   pd = (rot*pd).eval();
}

/** \ingroup Geometry
* Applies a Affine transformation to the screen extremal points
* @param affine Affine transformation
* The transformation of the points is irreversible so if many transformation are needed but without cumulative effect, one must call again
* setWidthHeight(), setOffset() and setFocalDistance()

* An example of using it at every frame during a simulation.
* \code
*  screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
*  screen.setOffset(alignmentX,alignmentY);
*  screen.setFocalDistance(_focalDist);
*  screen.transform( someTransformation );
* \endcode
**/
void Screen::transform( const Eigen::Affine3d &affine)
{  pa = (affine*pa).eval();
   pb = (affine*pb).eval();
   pc = (affine*pc).eval();
   pd = (affine*pd).eval();

}

/** \ingroup Geometry
* Return a copy 
*
**/
Screen Screen::getTransformed(const Eigen::Affine3d &A)
{
	Screen transformed(*this);
	transformed.transform(A);
	return transformed;
}

Vector3d Screen::getCenter() const
{
    return (pa+pb+pc+pd)/4;
}

/** \ingroup Geometry
* Print to output stream the screen extremal points
* @param os A valid output stream
**/
void Screen::print(ostream &os)
{  os << pa.transpose() << "\t" << pb.transpose() << "\t" << pc.transpose() << "\t" << pd.transpose() << endl;
}
