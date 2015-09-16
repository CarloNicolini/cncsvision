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

#include "GLUtils.h"
#include "MinJerkCurve.h"
#include "Mathcommon.h"

using namespace mathcommon;
using namespace Eigen;

/**
* \ingroup GLVisualization
*
* \brief Default constructor. Set radius=0.1, approachParameter=50, sphericalMode (approach a sphere instead of a cylinder).
**/
MinJerkCurve::MinJerkCurve()
{  radius = 0.1;
   approachParameter = 50;
   mode = sphereMode;
   phi = theta = 0;
   quad=NULL;
   cosphi = cos(phi);
   sinphi = sin(phi);
   costheta = cos(theta);
   sintheta = sin(theta);

}

/** \ingroup GLVisualization
* \brief  Init the curve with a specified number of points along the line and start-ending knots
*
* @param _numpoints Number of points along the curve between start and end points (typical is 100 )
* @param _mode Specify if we want to reach a sphere (mode=0) or a cylinder (mode=1). These values are defined in MinJerkCurve::sphereMode or MinJerkCurve::cylinderMode
* @param startKnot Starting point of the minimum jerk curve
* @param endKnot Ending point of the minimum jerk curve
**/
void MinJerkCurve::init( unsigned int _numpoints, int _mode,  const Vector3d &startKnot, const Vector3d &endKnot  )
{  numPoints = _numpoints;
   mode = _mode;
   start=startKnot;
   end=endKnot;
   //phi = unifRand(0.0, M_PI);
   theta = unifRand(0.0, 2*M_PI);

   phi = M_PI/2; // coordinata equatore ( latitudine sfera )
   theta = M_PI/4; // coordinata longitudine
   approachParameter = unifRand(0.1,10.0);

   cosphi = cos(phi);
   sinphi = sin(phi);
   costheta = cos(theta);
   sintheta = sin(theta);
}

/**
* \ingroup GLVisualization
* \brief Draw the precomputed curve. A rendering context must be active!
**/
void MinJerkCurve::draw()
{  Vector3d finger, thumb;

   glDisable(GL_LIGHTING);

   glEnable(GL_BLEND);

   glEnable(GL_LINE_SMOOTH);
   glLineWidth(3);

   glColor3fv(glWhite);
   glBegin(GL_LINE_STRIP);
   for ( double t=0; t<=1; t+=0.01 )
   {  finger = interpolateFinger(t);
      glVertex3dv(finger.data());
   }
   glEnd();

   glLineWidth(1);
   glDisable(GL_LINE_SMOOTH);
   glEnable(GL_LIGHTING);

   glPushMatrix();
   glTranslated(end.x(),end.y(),end.z());
   if (mode == sphereMode )
      glutWireSphere(radius,20,20);
   if ( mode == cylinderMode )
   {  double height=0.1;
      glRotated(90,1,0,0);
      glTranslated(0,0,-height/2);
      if (quad==NULL)
      {  quad = gluNewQuadric();
      }
      gluCylinder(quad, radius, radius, height, 32, 16 ); //base, top, height, slice, stacks

   }
   glPopMatrix();
}

/** \ingroup GLVisualization
* Get the finger position along the minimum jerk curve along at time t
* @param t The parameterized time \f$ t \in [0,1] \f$
* @return The point along the curve
**/
Vector3d MinJerkCurve::interpolateFinger( double t)
{  Vector3d final,A,R;
   double magic = 6*t*t -15*t + 10;
   double t3 = t*t*t;
   double a=approachParameter; // for comodity
   if (mode == sphereMode )
   {  A = Vector3d(a*costheta*sinphi,  a*cosphi, a*sintheta*sinphi);
      R = Vector3d(radius*costheta*sinphi,radius*cosphi, radius*sintheta*sinphi);
      final = start + ( A*0.5*(t-1)*(t-1) + (end - start + R )*magic )*t3;
   }
   if ( mode== cylinderMode )
   {  // here theta is the rotation angle around the cylinder
      A = Vector3d( a*costheta, 0, a*sintheta  );
      R = Vector3d( radius*costheta,  0 , radius*sintheta );
      final = start + ( A*0.5*(t-1)*(t-1) + (end - start + R )*magic )*t3;
   }
   return final;
}

/** \ingroup GLVisualization
* Draw the finger position along the minimum jerk curve.
* @param t The parameterized time \f$ t \in [0,1] \f$
**/
void MinJerkCurve::drawInterpolatedFinger(double t)
{  Vector3d tmp = interpolateFinger(t);
   glPushMatrix();
   glTranslated(tmp.x(),tmp.y(),tmp.z());
   glutSolidSphere(radius/5,20,20);
   glPopMatrix();
}

/** \ingroup GLVisualization
* Default destructor, clear the memory reserved from gluQuadric
**/
MinJerkCurve::~MinJerkCurve()
{  if (quad!=NULL)
      gluDeleteQuadric(quad);
}

