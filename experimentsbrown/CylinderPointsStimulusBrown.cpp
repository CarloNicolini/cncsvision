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

#include "CylinderPointsStimulusBrown.h"

//using namespace mathcommon;
/**
* \ingroup Stimulus
* Default constructor
* Set radius=height=fluffiness=1 startAngle =0 and endAngle=360°
**/
CylinderPointsStimulusBrown::CylinderPointsStimulusBrown() : PointwiseStimulus()
{  radius=1.0;
   height=1.0;
   fluffiness=1.0;
   startAngle = 0.0;
   endAngle = 2.0*M_PI;
   diameterX = 1.0;
   diameterZ = 1.0;
   test = 99.0;
}

/**
* \ingroup Stimulus
* Destructor
**/
CylinderPointsStimulusBrown::~CylinderPointsStimulusBrown()
{
}
/**
* Set the radius and the height of the cylinder. Height is along Y direction, radius is on XZ plane
* \param radius along x axis
* \param h height of the cylinder
**/
void CylinderPointsStimulusBrown::setRadiusAndHeight(double r, double h)
{  //radius = r;
   //height = h;
	setRadiusAndHeight(2*r,2*r,h);
}

/**
* \ingroup Stimulus
* Set the diameter along X, diameter along Z and height of the cylinder (a elliptic base cylinder in this case)
* \param diamX diameter along x axis
* \param diamZ diameter along z axis
* \param h height of the cylinder
**/
void CylinderPointsStimulusBrown::setRadiusAndHeight(double diamX, double diamZ, double h)
{
	diameterX=diamX;
	diameterZ=diamZ;
	height=h;
}

/**
* \ingroup Stimulus
* Set the angular aperture of the cylinder in radians
* \param _startAngle Starting angle in radians
* \param _endAngle End angle in radians
**/
void CylinderPointsStimulusBrown::setAperture(double _startAngle, double _endAngle )
{  startAngle = _startAngle;
   endAngle = _endAngle;
   compute();
}
/**
* \ingroup Stimulus
* Update the generating points
**/
void CylinderPointsStimulusBrown::compute()
{

   for(PointsRandIterator iPoint = pointsRand.begin(); iPoint != pointsRand.end(); ++iPoint )
   {  Point3D *p = *iPoint;
      double angle = mathcommon::unifRand(startAngle, endAngle);
      double lAxisNoise = diameterX/2 + mathcommon::gaussRand(0,fluffiness);
      double sAxisNoise = diameterZ/2 + mathcommon::gaussRand(0,fluffiness);
      double genX = sAxisNoise * (cos(angle));
      double genY = lAxisNoise * (sin(angle));
      p->x = genY;
      p->y = mathcommon::unifRand(-height/2,height/2);
      p->z =  genX;
   }
}

/**
 * @brief CylinderPointsStimulusBrown::getDiameterX
 * @return
 */
double CylinderPointsStimulusBrown::getDiameterX() const
{
    return this->diameterX;
}

/**
 * @brief CylinderPointsStimulusBrown::getDiameterZ
 * @return
 */
double CylinderPointsStimulusBrown::getDiameterZ() const
{
    return this->diameterZ;
}

/**
 * @brief CylinderPointsStimulusBrown::getHeight
 * @return
 */
double CylinderPointsStimulusBrown::getHeight() const
{
    return this->height;
}

/**
 * @brief CylinderPointsStimulusBrown::getTest
 * @return
 */
double CylinderPointsStimulusBrown::getTest() const
{
    return this->test;
}
