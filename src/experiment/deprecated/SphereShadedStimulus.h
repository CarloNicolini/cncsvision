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

#ifndef _SPHERESHADED_STIMULUS_H
#define _SPHERESHADED_STIMULUS_H

#include "ShadedStimulus.h"

/**
*       \class SphereShadedStimulus
*       \ingroup Stimulus
*       Defines a visual stimulus abstracting out the OpenGL visualization and only keeping the basic informations
*       A SphereShadedStimulus is a wrapper around the variables that define a sphere: radius and center
**/

class SphereShadedStimulus : public ShadedStimulus
{
public:
   double radius;

   SphereShadedStimulus();
   ~SphereShadedStimulus();

   void setRadius(double);
   
   // The method to override which are defined in the interface
   void compute();

   int getStimulusType()
   {  return Stimulus::StimSphereShaded ;
   };
   int isPointwise()
   {  return 0;
   };
};

#endif
