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

#include "CylinderShadedStimulus.h"
/**
* \ingroup Stimulus
* Default constructor
**/
CylinderShadedStimulus::CylinderShadedStimulus()
{  radius=1.0;
   height=0.5;
   startAngle=0.0;
   endAngle=2.0*M_PI;
}
/**
* \ingroup Stimulus
* Default destructor
**/
CylinderShadedStimulus::~CylinderShadedStimulus()
{

}
/**
* \ingroup Stimulus
* Set the radius of this cylinder
* \param r Radius
**/
void CylinderShadedStimulus::setRadius(double r)
{  radius = r;
}
/**
* \ingroup Stimulus
* Set the radius and height of this
* \param r Radius
* \param h Height
**/
void CylinderShadedStimulus::setRadiusAndHeight(double r, double h)
{  radius = r;
   h = height;
}
/**
* \ingroup Stimulus
* \param _startAngle Theta min
* \param _endAngle Theta max
**/
void CylinderShadedStimulus::setAperture(double _startAngle, double _endAngle )
{  startAngle = _startAngle;
   endAngle = _endAngle;
}


/**
* \ingroup Stimulus
* Update the stimulus
**/
void CylinderShadedStimulus::compute()
{


}

