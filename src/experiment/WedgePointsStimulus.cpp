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
#include "WedgePointsStimulus.h"

/**
* \ingroup Stimulus
* Default constructor
**/
WedgePointsStimulus::WedgePointsStimulus() :  width(1.0),height(1.0),depth(1.0),angle(M_PI/2),zOffset(0),nPoints(0)
{
}

/**
* \ingroup Stimulus
**/

WedgePointsStimulus::~WedgePointsStimulus()
{
}

/**
* \ingroup Stimulus
* \param _n Total number of points
**/
void WedgePointsStimulus::setNpoints(unsigned int _n)
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
    this->nPoints=_n;
    compute();
}

void WedgePointsStimulus::setAngle(double angle)
{
    this->angle = angle;
}


void WedgePointsStimulus::setPlanesSize(double edge)
{
    this->width = edge;
    this->depth = edge;
    this->height = 0.0;
}

void WedgePointsStimulus::setPlanesSize(double x, double z)
{
    this->width = x;
    this->depth = z;
    this->height = 0;
}

void WedgePointsStimulus::setZOffset(double zOffset)
{
    this->zOffset = zOffset;
}
/**
* \ingroup Stimulus
* Update the points in the stimulus, to be called after each new trial
**/
void WedgePointsStimulus::compute()
{
    int n=0;
    pointsRand.clear();
    double cotalpha = 1.0/tan(angle*0.5);
    double cotalpha2 = 1.0/tan(-angle*0.5);

    while ( n<nPoints )
    {
        double x = mathcommon::unifRand(-width*0.5,width*0.5);
        double y = mathcommon::unifRand(-width*0.5,width*0.5);
        double zUp =-y*cotalpha;
        double zDown =- y*cotalpha2;
        if (y>0)
        {
            pointsRand.push_back(new Point3D(x,y,zUp));
            n++;
        }
        if (y<0)
        {
            pointsRand.push_back(new Point3D(x,y,zDown));
            n++;
        }
    }
}
