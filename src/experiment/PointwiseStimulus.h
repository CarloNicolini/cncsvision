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

#ifndef _POINTWISE_STIMULUS_H
#define _POINTWISE_STIMULUS_H

#include <vector>
#include <map>
#include <Eigen/Core>

#include "Mathcommon.h"
#include "Stimulus.h"

/**
*       \class PointwiseStimulus
*       \ingroup Stimulus
*       \brief PointwiseStimulus is a class inherited from the interface stimulus
*  It's meant to represent a stimulus a set of points in the space, following some
*  geometric shape
*       Defines a visual stimulus abstracting out the OpenGL visualization and only keeping the basic informations,
*       that in this case are the number of points that define the visual stimulus.
*       A PointwiseStimulus is a set of points and this class specifies all the methods that get inherited from BoxNoiseStimulus, CubePointsStimulus, CylinderPointsStimulus.
**/
class PointwiseStimulus : public Stimulus
{

public:
   PointwiseStimulus();
   PointwiseStimulus(const PointwiseStimulus&);
   ~PointwiseStimulus();
   // Method related to points costituting the visual stimulus
   void setNpoints(unsigned int );
   void addPoint();

   void setFluffiness(double );    // to understand what we mean for "fluffinesss" look this http://3.bp.blogspot.com/-5OfQy7lYM-I/TbCU7Z5vSlI/AAAAAAAABTU/vIpjd__3vJE/s1600/very_fluffy_bunny1.jpg
   // Methods defined for inheritance purpose only
   void compute();

   // Identify the stimulus type: 0 for pointwise stimuli, 1 for shaded stimuli
   int getStimulusType()
   {  return Stimulus::StimPointwise ;
   };
   int isPointwise()
   {  return 1;
   };

   // Methods related to the PointwiseStimulus itself
   void shuffle();
   void print(ostream &);
   double fluffiness;
   
   PointsRand pointsRand;
   // The points special, dependent on the stimulus form and size (for example a Box Stimulus sets them to be the box vertices)
   Points specialPoints;
   PointsRand specialPointsRand;


     //standard deviation of the points around the surface of the generated cloud
};

#endif

