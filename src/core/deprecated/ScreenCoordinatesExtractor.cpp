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
#include <Eigen/StdVector>
#include <Eigen/Geometry>

#include "Mathcommon.h"

#include "ScreenCoordinatesExtractor.h"

using namespace mathcommon;
using namespace Eigen;

/**
* \ingroup Geometry
* \brief Constructor
**/
ScreenCoordinatesExtractor::ScreenCoordinatesExtractor()
{  intersectionAngle=0;
   intersectionLine.setZero();
   hasMarkerOffset=false;
   markerOffset=0.0; // in the same measurement units of the input points
}

/**
* \ingroup Geometry
* \brief Destructor, clear all the variables
**/
ScreenCoordinatesExtractor::~ScreenCoordinatesExtractor()
{  screenCoordinates.clear();
   virtualScreenCoordinates.clear();
}

/**
* \ingroup Geometry
* \brief Set the offset of the markers (usually 1 mm) to match the real marker position directly on the surface is attached onto.
* @param mOffset The offset in millimiters.
**/
void ScreenCoordinatesExtractor::setMarkerOffset(double mOffset )
{  hasMarkerOffset=true;
   markerOffset=mOffset;
}

// http://local.wasp.uwa.edu.au/~pbourke/geometry/planeplane/



/**
* \ingroup Geometry
* \brief Initialize this object with 3 screen coordinates and 3 mirror coordinates.
* Virtual screen coordinates are computed from the inputs given.
* \param _realScreen coordinates of 3 points on the screenCoord
* \param _realMirror coordinates of 3 points that define the mirror plane
**/
void ScreenCoordinatesExtractor::init( const vector<Vector3d> &_realScreen, const vector<Vector3d> &_realMirror )
{  
	if (_realScreen.size() != 3 || _realMirror.size() !=3 )
		throw std::runtime_error("I need exactly 3 points to define a plane!");

   virtualScreenCoordinates.resize(3);
   screenCoordinates.resize(3);
   for (int i=0; i<3; i++)
      screenCoordinates[i]=_realScreen[i];
   Hyperplane<double,3> screenPlane,mirrorPlane;

   screenPlane = Hyperplane<double,3>::Through( _realScreen[0],_realScreen[1],_realScreen[2] );
   mirrorPlane = Hyperplane<double,3>::Through( _realMirror[0],_realMirror[1],_realMirror[2] );

   if ( screenPlane.isApprox(mirrorPlane) )
     throw std::runtime_error("Planes are parallel!");
   
   // Save the planes informations for debug purposes
   mirrorOffset=mirrorPlane.offset();
   screenOffset=screenPlane.offset();
   mirrorNormal=mirrorPlane.normal();
   screenNormal=screenPlane.normal();

   // Correct the two planes for the marker offset (3.3 mm), translating them back along their normal of markerOffset
   if (hasMarkerOffset)
   {  Affine3d transScreen = Affine3d::Identity();
      Affine3d transMirror = Affine3d::Identity();
      transScreen.translate ( screenNormal*markerOffset );
      transMirror.translate ( mirrorNormal*markerOffset );
      screenPlane.transform(transScreen);
      mirrorPlane.transform(transMirror);
      //cerr << "SCREEN\n" << transScreen.matrix() << endl << "MIRROR\n" << transMirror.matrix() << endl;
   }

   for (int i=0; i<3; i++)
   {  Vector3d tmp = mirrorPlane.projection(Vector3d(screenCoordinates[i]));
      virtualScreenCoordinates[i] = Vector3d(tmp)*2 -screenCoordinates[i];

   }
}



/**
* \ingroup Geometry
* \brief Get the intersection angle between the screen and the mirror planes
* @return The intersection between the screen and the mirror planes
**/
double ScreenCoordinatesExtractor::getIntersectionAngle()
{  return toDegrees( acos( mirrorNormal.dot(screenNormal) ) ) ;
}

/**
* \ingroup Geometry
* \brief Get the angle of the mirror around the X axis. This angle should be 45 degrees
* @return The angle of the mirror around the X axis. This angle should be 45 degrees
**/
double ScreenCoordinatesExtractor::getMirrorAngleX()
{  return toDegrees(acos(mirrorNormal.dot(Vector3d::UnitX() )));
}

/**
* \ingroup Geometry
* \brief Get the angle of the mirror around the Y axis. This angle should be 90 degrees
* @return Get the angle of the mirror around the Y axis. This angle should be 90 degrees
**/
double ScreenCoordinatesExtractor::getMirrorAngleY()
{  return toDegrees(acos(mirrorNormal.dot(Vector3d::UnitY() )));
}

/**
* \ingroup Geometry
* \brief Get the distance between mirror and screen
* @return  Get the distance between mirror and screen
**/
double ScreenCoordinatesExtractor::getMirrorScreenDistance()
{  return mirrorOffset;
}

/**
* \ingroup Geometry
* \brief Get the screen normal
* @return  Screen normal vector
**/
Vector3d ScreenCoordinatesExtractor::getScreenNormal()
{  return Vector3d(screenNormal);
}

/**
* \ingroup Geometry
* \brief Get the normal vector (normalized ) of the mirror plane
* @return Mirror normal vector
**/
Vector3d ScreenCoordinatesExtractor::getMirrorNormal()
{  return Vector3d(mirrorNormal);
}

/**
* \ingroup Geometry
* \brief Return a reference to virtual screen coordinates obtained reflecting the
* real screen points through.
*
* \return realScreen coordinates of 3 points on the virtual screen
**/
vector<Vector3d> &ScreenCoordinatesExtractor::getVirtualCoordinates()
{  return virtualScreenCoordinates;
}

/**
* \ingroup Geometry
* Prints the virtual screen coordinates to specified output stream
*
* \param os Output stream
*
**/
void ScreenCoordinatesExtractor::print(ostream &os)
{  for (int i=0; i<3; i++)
   {  os << virtualScreenCoordinates[i] << endl;
   }
}
