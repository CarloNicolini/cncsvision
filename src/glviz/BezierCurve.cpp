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

#include <vector>
#include "GLUtils.h"
#include "BezierCurve.h"

using namespace std;
using namespace Eigen;

/**
 * \ingroup GLVisualization
 * Standard constructor, 3 control points
**/
BezierCurve::BezierCurve()
{  numPoints=3;
   distanceBetweenPoints=3;


}


/**
 * \ingroup GLVisualization
 * Init the Bezier curve with 3 knots.
 * A rendering context must be active!
 * \param knots Vector of curve control points
 **/
void BezierCurve::init( const vector<Vector3d> &knots )
{  if (knots.size() != 3)
   {  cerr << "We work only with 3 control points!" << endl;
      exit(0);
   }

   start=knots[0];
   control1=knots[1];
   end=knots[2];

   for (int i=0; i<3; i++)
   {  ctrlpoints[i][0] = knots[i][0];
      ctrlpoints[i][1] = knots[i][1];
      ctrlpoints[i][2] = knots[i][2];
   }

   glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, distanceBetweenPoints, numPoints, &ctrlpoints[0][0]);
   glEnable(GL_MAP1_VERTEX_3);
}

/**
 * \ingroup GLVisualization
 * Init a Bezier curve given 3 control points: start, control 1 and end point.
 * \param startKnot First control point
 * \param control1 half control point
 * \param endKnot last control point
**/
void BezierCurve::init( const Vector3d &startKnot, const Vector3d &control1, const Vector3d &endKnot  )
{  vector<Vector3d> knots;

   knots.push_back(startKnot);
   knots.push_back(control1);
   knots.push_back(endKnot);

   init(knots);
}

/**
 * \ingroup GLVisualization
 * Draw the initialized Bezier curve. A rendering context must be active.
**/

void BezierCurve::draw()
{  glDisable(GL_LIGHTING);
   glLineWidth(4);
   glMapGrid1f(100, 0.0, 1.0);
   glEvalMesh1(GL_LINE, 0, 100);

   glPointSize(10);
   glBegin(GL_POINTS);

   glVertex3fv(ctrlpoints[0]);
   glVertex3fv(ctrlpoints[2]);
   glVertex3fv(ctrlpoints[1]);
   glEnd();

   glEnable(GL_LIGHTING);

}


/**
 * \ingroup GLVisualization
 * Returns an interpolated point along the initialized bezier curve. The init method must be called first!
 * \param t The time \f$t \in [0,1] \f$
**/
Vector3d BezierCurve::interpolate( double t)
{  Vector3d  tmp = start*(1.0-t)*(1.0-t) + control1*2*(1-t)*t + end*t*t;
   return tmp;
}


/**
* \ingroup GLVisualization
* Default destructor
**/
BezierCurve::~BezierCurve()
{

}
