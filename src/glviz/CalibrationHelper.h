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


#ifndef _CALIBRATION_HELPER_H_
#define _CALIBRATION_HELPER_H_
#include <vector>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "GLUtils.h"
#include "Screen.h"
#include "IncludeGL.h"

/**
 * \class CalibrationHelper
 * \ingroup GLVisualization
 * \brief CalibrationHelper is an helper class to perform the monitor calibration. It offers basically three methods to check the calibration correctness:
 * <ul>
 * <li> A 10x10 cm grid drawn at focal distance, needed to check the XY proportions correctness </li>
 * <li> A 5x5x5 set of boxes aligned along the z-axis in order to check the Z depth correctness </li>
 * <li> A marker drawn with a GLPoint used to check the precision of calibration. </li>
 * <li> The FPS visualizer </li>
 * </ul>
 * An example of CalibrationHelper can be seen in the "funnydemos" examples in VC10 folder.
**/

class CalibrationHelper
{
public:
   CalibrationHelper();
   void setWidthHeightFocalPlane(int _w, int _h, double _focalPlaneDistance);
   void setScreen(const Screen &);

   void switchHeadCalibration();
   void switchInstructions();
   void switchCalibration();
   void switchFPS();
   void switchGrid();
   void switchBoxes();



   bool somethingToDraw();

   void addMarker(const Eigen::Vector3d &);

   void draw();
   void drawFPS(double);

private:

   std::vector<Eigen::Vector3d> markers;

   bool printHeadCalibration;
   bool printInstructions;
   bool printCalibration;
   bool printFPS;
   bool printGrid;
   bool printBoxes;
   int width, height;   //screen resolution in pixel, usually width=1024, height=768
   double focalPlaneDistance;
   int subwidth, subheight;
   Vector3d pa,pb,pc;
   double alignmentX,alignmentY;
};



#endif
