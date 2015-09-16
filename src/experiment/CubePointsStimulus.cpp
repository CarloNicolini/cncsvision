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

//#include <iostream>

#include "CubePointsStimulus.h"

/**
* \ingroup Stimulus
* Default constructor
**/
CubePointsStimulus::CubePointsStimulus() : edgeLength(1),nPoints(0),drawWhichFace(0)
{  fluffiness=1.0;
}
/**
* \ingroup Stimulus
**/
CubePointsStimulus::~CubePointsStimulus()
{

}

/**
* \ingroup Stimulus
* Set the edge length of the cube stimulus
* \param _e Edge length
**/
void CubePointsStimulus::setEdgeLength(double _e)
{  edgeLength=_e;
   compute();
}
/**
* \ingroup Stimulus
Place the points on the stimulus surfac
**/
void CubePointsStimulus::placePoints()
{  double fluff=edgeLength/(2.*fluffiness);
   
   PointsRandIterator iter;
   unsigned int k=0;

   for ( iter = pointsRand.begin(); iter!=pointsRand.end();)
   {  switch (k%6)
      {  case 0:
         {  (*iter)->set( mathcommon::unifRand(-edgeLength,edgeLength), mathcommon::unifRand(-edgeLength,edgeLength), mathcommon::gaussRand(-edgeLength/fluff,edgeLength/fluff) + edgeLength );
            ++iter;
         }
         break;
         case 1:
         {  (*iter)->set( mathcommon::unifRand(-edgeLength,edgeLength), mathcommon::unifRand(-edgeLength,edgeLength), mathcommon::gaussRand(-edgeLength/fluff,edgeLength/fluff) - edgeLength );
            ++iter;
         }
         break;
         case 2:
         {  (*iter)->set( mathcommon::gaussRand(-edgeLength/fluff,edgeLength/fluff) - edgeLength, mathcommon::unifRand(-edgeLength,edgeLength), mathcommon::unifRand(-edgeLength,edgeLength)  );
            ++iter;
         }
         break;
         case 3:
         {  (*iter)->set( mathcommon::gaussRand(-edgeLength/fluff,edgeLength/fluff) + edgeLength, mathcommon::unifRand(-edgeLength,edgeLength), mathcommon::unifRand(-edgeLength,edgeLength)  );
            ++iter;
         }
         break;
         case 4:
         {  (*iter)->set( mathcommon::unifRand(-edgeLength,edgeLength), mathcommon::gaussRand(-edgeLength/fluff,edgeLength/fluff) + edgeLength, mathcommon::unifRand(-edgeLength,edgeLength)  );
            ++iter;
         }
         break;
         case 5:
         {  (*iter)->set( mathcommon::unifRand(-edgeLength,edgeLength), mathcommon::gaussRand(-edgeLength/fluff,edgeLength/fluff) - edgeLength, mathcommon::unifRand(-edgeLength,edgeLength)  );
            ++iter;
         }
         break;
      }
      k++;
   }
}

/**
* \ingroup Stimulus
* Update the stimulus
**/
void CubePointsStimulus::compute()
{  placePoints();

}
