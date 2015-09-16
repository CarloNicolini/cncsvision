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

#include "SpherePointsStimulus.h"
/**
* \ingroup Stimulus
* Default constructor
**/
SpherePointsStimulus::SpherePointsStimulus() : radius(1.0)
{  fluffiness=1.0;
}

/**
* \ingroup Stimulus
* Default destructor
**/
SpherePointsStimulus::~SpherePointsStimulus()
{

}
/**
* \ingroup Stimulus
* \brief Set the radius of the stimulus
* \param r radius in arbitrary units.
**/
void SpherePointsStimulus::setRadius(double r)
{  radius = r;
}



/**
* \ingroup Stimulus
* \brief Compute the points position as defined by stimulus type
**/
void SpherePointsStimulus::compute()
{
      for(PointsRandIterator iPoint = pointsRand.begin(); iPoint != pointsRand.end(); ++iPoint )
	  {  Point3D *p = *iPoint;
         p->x = mathcommon::gaussRand(0,1);
         p->y = mathcommon::gaussRand(0,1);
         p->z = mathcommon::gaussRand(0,1);
         p->normalize();
         (*p)*=radius;
         (*p)+= (Point3D(mathcommon::gaussRand(0.,fluffiness),mathcommon::gaussRand(0.,fluffiness),mathcommon::gaussRand(0.0, fluffiness)));
      }
}
