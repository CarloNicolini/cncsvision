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

#ifndef _SHADED_STIMULUS_H
#define _SHADED_STIMULUS_H

#include "Stimulus.h"

/**
*       \class ShadedStimulus
*       \ingroup Stimulus
*       ShadedStimulus is a class inherited from the interface Stimulus
*  It's meant to represent a stimulus a shaded polytope
**/
class ShadedStimulus : public Stimulus
{

public:
   ShadedStimulus();
   ShadedStimulus(const ShadedStimulus&);
   virtual ~ShadedStimulus();
   // Method related to points costituting the visual stimulus

   void setFluffiness(double );
   // Methods defined for inheritance purpose only
   void compute();

   int getStimulusType()
   {  return Stimulus::StimShaded;
   };
   int isPointwise()
   {  return 0;
   };
   // Methods related to the ShadedStimulus itself
   void print(ostream &);
   //void setScale(double ax, double ay, double az);
   double scaleX,scaleY,scaleZ;
};

#endif

