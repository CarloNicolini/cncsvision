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

#ifndef _BOUND_CHECKER_H_
#define _BOUND_CHECKER_H_
#include <vector>
#include "IncludeGL.h"

#include "Point3D.h"
#include "Stimulus.h"
#include "PointwiseStimulus.h"
#include "SpherePointsStimulus.h"
#include "CubePointsStimulus.h"
#include "CylinderPointsStimulus.h"
#include "BoxNoiseStimulus.h"
#include "VRCamera.h"
#include "GLUtils.h"

class BoundChecker
{

public:
   BoundChecker();
   BoundChecker(Stimulus *_stim);
   BoundChecker(VRCamera *_cam, Stimulus *_stim);

   void setCameraAndStimulus(VRCamera *_cam, Stimulus *_stim);
   bool checkOutside(Affine3d &objectTransformation);

   VRCamera *cam;
   Stimulus *currentStimulus;
};

#endif
