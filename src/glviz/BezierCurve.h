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

#ifndef _BEZIER_H_
#define _BEZIER_H_

#include "IncludeGL.h"
#include <vector>
#include <Eigen/Core>


using namespace Eigen;

/**
 * \class BezierCurve
 * \defgroup GLVisualization
 * \ingroup GLVisualization
 * \brief Bezier curve is a helper class to draw second order Bezier curve, given three control points.
 *
**/

class BezierCurve
{
public:
   BezierCurve();
   ~BezierCurve();
   void init( const std::vector < Vector3d >  &knots );
   void init( const Vector3d &start, const Vector3d &control1, const Vector3d &endKnot  );
   void draw();
   Vector3d interpolate( double t );
protected:
   Vector3d start, end, control1;
   int numPoints;
   float distanceBetweenPoints;
   float ctrlpoints[3][3];

};

#endif
