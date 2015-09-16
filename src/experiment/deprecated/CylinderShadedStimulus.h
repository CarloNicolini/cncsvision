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

#ifndef _CYLINDERSHADED_STIMULUS_H
#define _CYLINDERSHADED_STIMULUS_H

#include "ShadedStimulus.h"

/**
*       \class CylinderShadedStimulus
*       \ingroup Stimulus
*       CylinderShadedStimulus is a class inherited from the interface Stimulus
*  It's meant to represent a shaded cylindric object
**/
class CylinderShadedStimulus : public ShadedStimulus
{
public:
   double radius;
   double height;
   double startAngle, endAngle;

   CylinderShadedStimulus();
   ~CylinderShadedStimulus();

   void setRadius(double);
   void setCenter(const Point3D &);
   void setCenter(double, double, double);
   void setRadiusAndHeight(double, double);
   void setAperture(double, double);

   // The method to override which are defined in the interface
   void compute();

   int getStimulusType()
   {  return Stimulus::StimCylinderShaded ;
   };
   int isPointwise()
   {  return 0;
   };
};

#endif
