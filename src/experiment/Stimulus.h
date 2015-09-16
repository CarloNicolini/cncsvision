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

#ifndef _STIMULUS_H
#define _STIMULUS_H

#include "Point3D.h"
#include "Mathcommon.h"
/**
* \class Stimulus
* \defgroup Stimulus Stimulus
* \ingroup Stimulus
* This is the interface for every kind of visual stimulus to present
*
**/


class Stimulus
{
public:
   virtual ~Stimulus() {  };
   /**
   * \ingroup Stimulus
   * Update the stimulus
   **/
   virtual void compute()=0;

   /**
   * \ingroup Stimulus
   * Get the stimulus type:
   * StimPointwise=0
   * StimBoxPointwise=1
   * StimSpherePointwise=2
   * StimCylinderPointwise=3
   * StimBoxNoisePointwise = 4
   * StimRadialPointwise = 5
   * StimShaded = 6
   * StimBoxShaded = 7
   * StimSphereShaded = 8
   * StimCylinderShaded = 9
   **/
   virtual int getStimulusType() = 0;
   virtual int isPointwise() = 0;

   static const int StimPointwise=0;
   static const int StimBoxPointwise = 1;
   static const int StimSpherePointwise = 2;
   static const int StimCylinderPointwise = 3;
   static const int StimBoxNoisePointwise = 4;
   static const int StimRadialPointwise = 5;
   static const int StimEllipsoidPointwise = 6;
   static const int StimParaboloidPointwise = 7;
   static const int StimWedgePointwise = 8;

   static const int StimShaded = 9;
   static const int StimBoxShaded = 10;
   static const int StimSphereShaded = 11;
   static const int StimCylinderShaded = 12;
};

#endif
