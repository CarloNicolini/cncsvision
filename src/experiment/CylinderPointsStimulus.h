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

#ifndef _CYLINDER_STIMULUS_H
#define _CYLINDER_STIMULUS_H

#include "PointwiseStimulus.h"

/**
*       \class CylinderPointsStimulus
*       \ingroup Stimulus
*       Defines a visual stimulus abstracting out the OpenGL visualization and only keeping the basic informations,
*       that in this case are the number of points that define the visual stimulus.
*       A CylinderPointsStimulus is a set of points that stay on a cylindrical surface defined by width, radius, height, and aperture.
**/

class CylinderPointsStimulus : public PointwiseStimulus
{
public:
   double height, startAngle, endAngle, radius, diameterX,diameterZ;

   CylinderPointsStimulus();
   ~CylinderPointsStimulus();

   void setRadiusAndHeight(double radius, double height);
   void setRadiusAndHeight(double diameterX, double diameterZ, double height);
   void setAperture(double startAngle, double endAngle);
   double getDiameterX() const;
   double getDiameterZ() const;
   double getHeight() const;

   // The method to override which are defined in the interface
   void compute();

   int getStimulusType()
   {  return Stimulus::StimCylinderPointwise ;
   };
   int isPointwise()
   {  return 1;
   };
};

#endif
