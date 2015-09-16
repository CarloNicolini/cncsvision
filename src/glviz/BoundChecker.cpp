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

#include "BoundChecker.h"
#include "Mathcommon.h"
#include "Stimulus.h"
#include "PointwiseStimulus.h"
#include "VRCamera.h"

using namespace mathcommon;


BoundChecker::BoundChecker()
{  cam=NULL;
   currentStimulus=NULL;
}

BoundChecker::BoundChecker(Stimulus *_stim)
{  currentStimulus=_stim;
   cam=NULL;
}

BoundChecker::BoundChecker(VRCamera *_cam, Stimulus *_stim)
{  cam=_cam;
   currentStimulus=_stim;
}

bool BoundChecker::checkOutside(Affine3d &objectTransformation)
{  PointwiseStimulus *pstim = (PointwiseStimulus*)currentStimulus;
   PointsRandIterator pEnd = pstim->pointsRand.end();
   if ( pstim == NULL || cam==NULL ) return false;

   double winx=0,winy=0,winz=0;
   Point3D *p=NULL;

   bool somePointIsOutside=false;
   for (PointsRandIterator iRand = pstim->specialPointsRand.begin(); iRand!=pstim->specialPointsRand.end(); ++iRand)
   {  p=(*iRand);
      Vector3d v = objectTransformation*Vector3d( p->x, p->y, p->z);
      gluProject(v.x(),v.y(),v.z(), cam->getModelViewMatrix().data(), cam->getProjectiveMatrix().data(), cam->getViewport().data(), &winx,&winy,&winz);
      if ( (winx < 0 || winx > cam->getViewport()(2)) || (winy < 0 || winy > cam->getViewport()(3)) )
      {  somePointIsOutside=true;
         continue;
      }
   }
   return somePointIsOutside;
}

