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

#ifndef _ScreenCoordinatesExtractor_
#define _ScreenCoordinatesExtractor_

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace std;
using namespace Eigen;
/**
*       \class ScreenCoordinatesExtractor
*       \ingroup Geometry
*       \brief This class is an utility for the screen - mirror pair calibration.
*       Given a set of 3 points attached to the screen and 3 points attached
*       to the mirror, it computes the relative screen coordinates,
*       reflecting the points on the screen with respect to the mirror.
**/
class ScreenCoordinatesExtractor
{
public:
   ScreenCoordinatesExtractor();
   ~ScreenCoordinatesExtractor();
   void init( const std::vector<Vector3d> &allPoints, std::set<int> pointsID );
   void init( const std::vector<Vector3d> &realScreen, const std::vector<Vector3d> &realMirror);

   void print(ostream &);

   void setMarkerOffset(double mOffset );

   std::vector<Vector3d>& getVirtualCoordinates();
   double getIntersectionAngle();
   double getMirrorAngleX();
   double getMirrorAngleY();
   double getMirrorScreenDistance();
   Vector3d getScreenNormal();
   Vector3d getMirrorNormal();
private:
   std::vector<Vector3d> screenCoordinates;

   std::vector<Vector3d> virtualScreenCoordinates;

   Vector3d intersectionLine;
   double intersectionAngle;

   double mirrorOffset;
   double screenOffset;
   Vector3d mirrorNormal;
   Vector3d screenNormal;

   bool hasMarkerOffset;
   double markerOffset;


};
#endif
