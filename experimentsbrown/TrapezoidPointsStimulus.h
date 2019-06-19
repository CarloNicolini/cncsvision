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

#ifndef _EXTRUDEDTRAPEZE_POINTS_STIMULUS_H
#define _EXTRUDEDTRAPEZE_POINTS_STIMULUS_H

#include "PointwiseStimulus.h"
#include "Triangle3.h"

/**
*       \class TrapezoidPointsStimulus
*       \ingroup Stimulus
*       Defines a visual stimulus abstracting out the OpenGL visualization and only keeping the basic informations
*       A TrapezoidPointsStimulus is a set of points, on two planes that make two trapezes. The trapeze base is at 0 the back is at -z.
**/

class TrapezoidPointsStimulus : public PointwiseStimulus
{
public:
    unsigned int nPoints;

    double heightNear;
    double heightFar;
	double wNear;
    double wFar;
    double zNear;
    double zFar;
	Vector3d center;
    double pointsDensity;

    TrapezoidPointsStimulus();
    ~TrapezoidPointsStimulus();

    void setPointsDensity();
    // The method to override which are defined in the interface
    void compute();

	void setCenter(const Vector3d &c);
    void setHeightNear(double value);
    void setHeightFar(double value);
    void setWNear(double value);
    void setWFar(double value);
    void setZNear(double value);
    void setZFar(double value);
    void setDensity(double value);
    void setNpoints(unsigned int value);
    int getStimulusType()
    {  return Stimulus::StimWedgePointwise;
    }

    int isPointwise()
    {  return 1;
    }

    Vector3d *getVertices();

private:
    PointsRand pointsInQuadrilateral(const Vector3d &P0, const Vector3d &P1, const Vector3d &P2, const Vector3d &P3, double density);
    Vector3d P[8];
    double quadrilateralArea(const Vector3d &P0, const Vector3d &P1, const Vector3d &P2, const Vector3d &P3);
};



#endif
