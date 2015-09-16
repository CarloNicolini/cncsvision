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

#define _USE_MATH_DEFINES
#include <math.h>

#include "EllipsoidPointsStimulus.h"

//using namespace mathcommon;
/**
* \ingroup Stimulus
* Default constructor
**/
EllipsoidPointsStimulus::EllipsoidPointsStimulus() : PointwiseStimulus()
{  xAxis=1.0;
   zAxis=1.0;
   heightY=1.0;
   fluffiness=1.0;
   startAngle = 0.0;
   endAngle = 2.0*M_PI;
   theta = 0.0;
}

/**
* \ingroup Stimulus
**/
EllipsoidPointsStimulus::~EllipsoidPointsStimulus()
{
}
/**
* \ingroup Stimulus
* Set the main ellipsoid axes length
* \param s Short axis length
* \param l Long axis length
* \param h Height axis length
**/
void EllipsoidPointsStimulus::setAxesAndHeight(double _xAxis, double _zAxis, double _heightY)
{  xAxis = _xAxis /2;
   zAxis = _zAxis/2;
   heightY = _heightY;
}

double EllipsoidPointsStimulus::getHeight() const
{
	return this->heightY;
}

double EllipsoidPointsStimulus::getZAxis() const
{
	return this->zAxis;
}

double EllipsoidPointsStimulus::getXAxis() const
{
	return this->xAxis;
}

/**
* \ingroup Stimulus
* Set the angular aperture of the stimulus
* \param _startAngle Theta min
* \param _endAngle Theta max
**/
void EllipsoidPointsStimulus::setAperture(double _startAngle, double _endAngle )
{  startAngle = _startAngle;
   endAngle = _endAngle;
   compute();
}
/**
* \ingroup Stimulus
* Update the stimulus laying out the points in its volume
**/
void EllipsoidPointsStimulus::compute()
{

   for(PointsRandIterator iPoint = pointsRand.begin(); iPoint != pointsRand.end(); ++iPoint )
   {  Point3D *p = *iPoint;
      double angle = mathcommon::unifRand(startAngle, endAngle);
      double zAxisNoise = zAxis + mathcommon::gaussRand(0,fluffiness);
      double xAxisNoise = xAxis + mathcommon::gaussRand(0,fluffiness);
      double genX = xAxisNoise * (cos(angle));
      double genZ = zAxisNoise * (sin(angle));
      p->x = genX * (cos(theta)) - genZ * (sin(theta));
      p->y = mathcommon::unifRand(-heightY/2,heightY/2);
      p->z = genX * (sin(theta)) + genZ * (cos(theta));
   }
}
