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
#include "ExtrudedTrapezePointsStimulus.h"


void ExtrudedTrapezePointsStimulus::setNpoints(unsigned int value)
{
    this->nPoints = value;
}

void ExtrudedTrapezePointsStimulus::setHeight(double value)
{
    height = value;
}

void ExtrudedTrapezePointsStimulus::setWNear(double value)
{
    wNear = value;
}

void ExtrudedTrapezePointsStimulus::setWFar(double value)
{
    wFar = value;
}

void ExtrudedTrapezePointsStimulus::setZNear(double value)
{
    zNear = value;
}

void ExtrudedTrapezePointsStimulus::setZFar(double value)
{
    zFar = value;
}

ExtrudedTrapezePointsStimulus::ExtrudedTrapezePointsStimulus() :  nPoints(0)
{
}

/**
* \ingroup Stimulus
**/

ExtrudedTrapezePointsStimulus::~ExtrudedTrapezePointsStimulus()
{
}

double ExtrudedTrapezePointsStimulus::quadrilateralArea(const Vector3d &P0, const Vector3d &P1, const Vector3d &P2, const Vector3d &P3)
{
    pair<Triangle3,Triangle3> tris =triangulate3(P0,P1,P2,P3);
    return tris.first.area() + tris.second.area();
}
#include "Mathcommon.h"
using mathcommon::unifRand;
PointsRand ExtrudedTrapezePointsStimulus::pointsInQuadrilateral(const Vector3d &P0,const Vector3d &P1,const Vector3d &P2,const Vector3d &P3, double density)
{
    PointsRand points;
    vector<Vector3d> pp;
    pp.push_back(P0);
    pp.push_back(P1);
    pp.push_back(P2);
    pp.push_back(P3);

    double xmax=std::numeric_limits<double>::min();
    double xmin=std::numeric_limits<double>::max();

    double ymax=std::numeric_limits<double>::min();
    double ymin=std::numeric_limits<double>::max();

    double zmax=std::numeric_limits<double>::min();
    double zmin=std::numeric_limits<double>::max();

    for(int i=0; i<4;i++)
    {
        xmax = std::max(pp.at(i).x(),xmax);
        ymax = std::max(pp.at(i).y(),ymax);
        zmax = std::max(pp.at(i).z(),zmax);

        xmin = std::min(pp.at(i).x(),xmin);
        ymin = std::min(pp.at(i).y(),ymin);
        zmin = std::min(pp.at(i).z(),zmin);
    }

    pair<Triangle3,Triangle3> tri = triangulate3(P0,P1,P2,P3);
    unsigned int nPointsInQuadrilateral = static_cast<unsigned int>(density*quadrilateralArea(P0,P1,P2,P3));
    unsigned int k=0;
    while ( k< nPointsInQuadrilateral)
    {
        Vector3d p(unifRand(xmin,xmax),unifRand(ymin,ymax),unifRand(zmin,zmax));
        if ( tri.first.contains_projection(p))
        {
            points.push_back(new Point3D(tri.first.projected(p)));
            ++k;
        }
        if (tri.second.contains_projection(p))
        {
            points.push_back(new Point3D(tri.second.projected(p)));
            ++k;
        }
    }
    return points;
}

void ExtrudedTrapezePointsStimulus::setDensity(double value)
{
    this->pointsDensity = value;
}

void ExtrudedTrapezePointsStimulus::setCenter(const Vector3d &c)
{
    this->center << c;
}

/**
* \ingroup Stimulus
* Update the points in the stimulus, to be called after each new trial
**/
void ExtrudedTrapezePointsStimulus::compute()
{
    for(PointsRandIterator iPoint = pointsRand.begin(); iPoint != pointsRand.end(); ++iPoint )
    {
        if (*iPoint!=NULL)
            delete *iPoint;
    }

    this->pointsRand.clear();
    /**
          ^ (y)
         /
        /
        -----> [x]
        |
        |
        |
        v [z]

        (front view)
        4____5______6____7
        |    |      |    |
        |    |      |    |
        |    |      |    |
        0____1______2____3

        (bottom view)
        0--------------------3
        \                  /
         \                /
          \1____________2/

        (top view)
        4--------------------7
        \                  /
         \                /
          \5____________6/
    **/
    P[0] << -wFar/2,  -height/2, zFar;
    P[1] << -wNear/2, -height/2, zNear;
    P[2] <<  wNear/2, -height/2, zNear;
    P[3] <<  wFar/2,  -height/2, zFar;

    P[4] << -wFar/2,  height/2, zFar;
    P[5] << -wNear/2, height/2, zNear;
    P[6] <<  wNear/2, height/2, zNear;
    P[7] <<  wFar/2,  height/2, zFar;

    // Translates to center
    for (int i=0; i<8; i++)
        P[i] += this->center;

    // Front face
    PointsRand pointsFaceFront = this->pointsInQuadrilateral(P[1],P[2],P[6],P[5],pointsDensity);
    this->pointsRand.insert(this->pointsRand.end(),pointsFaceFront.begin(),pointsFaceFront.end());
    
    // Bottom face (trapeze)
    PointsRand pointsFaceBottom = this->pointsInQuadrilateral(P[0],P[1],P[2],P[3],pointsDensity);
    this->pointsRand.insert(this->pointsRand.end(),pointsFaceBottom.begin(),pointsFaceBottom.end());

    // Top face (trapeze)
    PointsRand pointsFaceTop = this->pointsInQuadrilateral(P[4],P[5],P[6],P[7],pointsDensity);
    this->pointsRand.insert(this->pointsRand.end(),pointsFaceTop.begin(),pointsFaceTop.end());

    // Left face
    PointsRand pointsFaceLeft = this->pointsInQuadrilateral(P[0],P[1],P[5],P[4],pointsDensity);
    this->pointsRand.insert(this->pointsRand.end(),pointsFaceLeft.begin(),pointsFaceLeft.end());

    // Right face
    PointsRand pointsFaceRight = this->pointsInQuadrilateral(P[2],P[3],P[7],P[6],pointsDensity);
    this->pointsRand.insert(this->pointsRand.end(),pointsFaceRight.begin(),pointsFaceRight.end());

    //std::random_shuffle(pointsRand.begin(),pointsRand.end());
}


Vector3d *ExtrudedTrapezePointsStimulus::getVertices()
{
    return this->P;
}
