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

#ifndef _SPHERE_STIMULUS_H
#define _SPHERE_STIMULUS_H

#include "PointwiseStimulus.h"

/**
*       \class SpherePointsStimulus
*       \ingroup Stimulus
*       Defines a visual stimulus abstracting out the OpenGL visualization and only keeping the basic informations,
*       that in this case are the number of points that define the visual stimulus.
*       A SpherePointsStimulus is a set of points that stay on a sphere with a given standard deviation around its surface
**/

class SpherePointsStimulus : public PointwiseStimulus
{
   double radius;
public:
   SpherePointsStimulus();
   ~SpherePointsStimulus();

   void setRadius(double);
   
   // The method to override which are defined in the interface
   void compute();

   int getStimulusType()
   {  return Stimulus::StimSpherePointwise ;
   };
   int isPointwise()
   {  return 1;
   };

};

#endif
