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

#ifndef _WEDGE_POINTS_STIMULUS_H
#define _WEDGE_POINTS_STIMULUS_H

#include "PointwiseStimulus.h"

/**
*       \class WedgePointsStimulus
*       \ingroup Stimulus
*       Defines a visual stimulus abstracting out the OpenGL visualization and only keeping the basic informations
*       A WedgePointsStimulus is a set of points, on two planes that make a wedge. The wedge is at 0, the rest is back
**/
class WedgePointsStimulus : public PointwiseStimulus
{
public:
   unsigned int nPoints;
   double width, height, depth;
   double angle;
   double zOffset;

   WedgePointsStimulus();
   ~WedgePointsStimulus();

   void setAngle(double angle);
   void setZOffset(double zOffset);
   void setPlanesSize(double x, double z);
   void setPlanesSize(double edge);
   void setNpoints(unsigned int _n);

   // The method to override which are defined in the interface
   void compute();
   int getStimulusType()
   {  return Stimulus::StimWedgePointwise;
   };
   int isPointwise()
   {  return 1;
   };
};

#endif
