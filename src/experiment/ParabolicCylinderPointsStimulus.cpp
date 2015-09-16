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

#include "ParabolicCylinderPointsStimulus.h"

//using namespace mathcommon;
/**
* \ingroup Stimulus
* Default constructor
**/
ParabolicCylinderPointsStimulus::ParabolicCylinderPointsStimulus() : PointwiseStimulus()
{
    domainRadiusXZ=1.0;
    curvatureY=1.0;
    fluffiness=1.0;
}

/**
* \ingroup Stimulus
**/
ParabolicCylinderPointsStimulus::~ParabolicCylinderPointsStimulus()
{
}

/**
 * @brief ParabolicCylinderPointsStimulus::setCurvature
 * @param _curvatureY
 */
void ParabolicCylinderPointsStimulus::setCurvature(double _curvatureY)
{
    this->curvatureY=_curvatureY;
}

/**
 * @brief ParabolicCylinderPointsStimulus::setDomainXZ
 * @param domainRadius
 */
void ParabolicCylinderPointsStimulus::setDomainXZ(double domainRadius)
{
    this->domainRadiusXZ=domainRadius;
}

/**
* \ingroup Stimulus
* Update the stimulus laying out the points in its volume
**/
void ParabolicCylinderPointsStimulus::compute()
{
    // Do not change this equation, the points are randomly generated in a circle
    // the polar equation is not correct for this kind of stimuli
    double d2=domainRadiusXZ*domainRadiusXZ;
    for ( PointsRandIterator iPoint = pointsRand.begin(); iPoint != pointsRand.end(); )
    {
        double x = mathcommon::unifRand(-domainRadiusXZ,domainRadiusXZ);
        double y = mathcommon::unifRand(-domainRadiusXZ,domainRadiusXZ);

        Point3D *p = *iPoint;
        p->x = x;
        p->y = y;
        p->z = -(curvatureY*(p->x*p->x)-curvatureY);
        ++iPoint;
    }
}
