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

//#include "Mathcommon.h"
#include "BoxNoiseStimulus.h"

/**
* \ingroup Stimulus
* Default constructor
**/
BoxNoiseStimulus::BoxNoiseStimulus() :  width(1.0),height(1.0),depth(1.0)
{
}

/**
* \ingroup Stimulus
**/

BoxNoiseStimulus::~BoxNoiseStimulus()
{
}

/**
* \ingroup Stimulus
* \param _n Total number of points
**/
void BoxNoiseStimulus::setNpoints(unsigned int _n)
{
    PointwiseStimulus::setNpoints(_n);
    unsigned int nSpecialPoints=13;
    if (specialPointsRand.size()!=13)
    {
        for ( unsigned int i=0; i<nSpecialPoints; i++)
        {  Point3D *p = new Point3D();
            specialPointsRand.push_back(p);
        }
    }
    compute();
}
/**
* \ingroup Stimulus
* \param _x Dimension on X axis
* \param _y Dimension on Y axis
* \param _z Dimension on Z axis
**/
void BoxNoiseStimulus::setDimensions(double _x, double _y, double _z)
{  width = _x;
    height = _y;
    depth = _z;
    compute();
}

/**
* \ingroup Stimulus
* \param e Set the edge length for a cubic stimulus
**/
void BoxNoiseStimulus::setDimensions(double e)
{  width = height = depth = e ;
    compute();
}


/**
* \ingroup Stimulus
* Update the points in the stimulus, to be called after each new trial
**/
void BoxNoiseStimulus::compute()
{  for(PointsRandIterator iPoint = pointsRand.begin(); iPoint != pointsRand.end(); ++iPoint )
    {  Point3D *p = *iPoint;
        p->x = mathcommon::unifRand(-width/2.0,width/2.0);
        p->y = mathcommon::unifRand(-height/2.0,height/2.0);
        p->z = mathcommon::unifRand(-depth/2.0,depth/2.0);
    }

    if ( specialPointsRand.size()!=13 )
    {  //std::cerr << "Extremal points must be 13!" << endl;
    }
    else
    {  // Vertices on the extremal points
        specialPointsRand[0]->set(-width/2, -height/2,0);
        specialPointsRand[1]->set(-width/2, height/2,0);
        specialPointsRand[2]->set(width/2, -height/2,0);
        specialPointsRand[3]->set(width/2, height/2,0);

        // center
        specialPointsRand[4]->set(0,0,0);

        // Top edge center
        specialPointsRand[5]->set(0,height/2,0);

        // Low edge center
        specialPointsRand[6]->set(0,-height/2,0);

        // Right edge center
        specialPointsRand[7]->set(width/2,0,0);

        // Left edge center[0]
        specialPointsRand[8]->set(-width/2,0,0);

        // Middle points
        specialPointsRand[9]->set(0,height/4,0);
        specialPointsRand[10]->set(0,-height/4,0);
        specialPointsRand[11]->set(width/4,0,0);
        specialPointsRand[12]->set(-width/4,0,0);
    }
}


// TO DEPRECATE 
void BoxNoiseStimulus::setSlantTilt(double _slant, int _tilt)
{  double tanslant =  tan(_slant);
    double sinTilt  =  sin(mathcommon::toRadians( (double) _tilt ));
    double costTilt =  cos(mathcommon::toRadians( (double) _tilt ));

    for(PointsRandIterator iPoint = pointsRand.begin(); iPoint != pointsRand.end(); ++iPoint )
    {  Point3D *p = *iPoint;
        p->x = mathcommon::unifRand(-width/2.0,width/2.0);
        p->y = mathcommon::unifRand(-height/2.0,height/2.0);
        p->z = tanslant*costTilt*(p->x) + tanslant*sinTilt*(p->y);
    }

}

