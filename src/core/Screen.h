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


#ifndef _SCREEN_H
#define _SCREEN_H

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace std;

/**
* \class Screen
* \ingroup Geometry
* \brief This class is very useful when you wanto to calibrate the screen-optotrak system.
* In order to get a correct alignment of the two reference frames, the real and the virtual world,
* must be adjusted. Here we suppose that the two frames differ only by a translation+scaling factor
* In a usual program you should make the variable screen global and set it at the very beginning of everything.
*
*
* Example of usage of \ref Screen are listed in \code VC10/examples/screenOptotrakCalibrator.cpp \endcode.
* Here we give you some hints :
*
* \code
* Screen screen; // this set the default screen width=height=focalDistance=100
* screen.loadParameterFile("calibration.txt");
* \endcode
* Et voilà, the calibration is done. What you must keep in mind now is that the file \code calibration.txt \endcode must be correct.
* A simple case is the following:
*
* <ul>
* <li> Virtual screen is reflected through the reflecting glass
* <li> The virtual image is in front of you at a distance z=-418.5 mm
* <li> 1024x768 resolution.
* <li> You measured the size of the horizontal edge to be 310mm
* <li> The virtual screen normal is parallel to \f$ \hat{z} \f$-axis of optotrak frame.
* </ul>
* Well, this is everything you need to make a calibration.
* Create a file named "calibration_today.txt" with the following content:
* \verbatim
% Calibration file 05/Aprile/2010
% Comments start with a '#'
% These are the variables to be set for a correct screen-optotrak alignment
% Resolution 1024x768
% A classical setup should be the following
% #define SCREEN_WIDTH  1024      // pixels
% #define SCREEN_HEIGHT 768       // pixels
% #define SCREEN_WIDE_SIZE 310.0    // millimeters

Width: 310.0
Height: 240.0
FocalDistance: 418.5
OffsetX: 155.0
OffsetY: -120.0
\endverbatim
*
* The % are needed to insert put comments, like date or experimental situation.
* The measures are in millimeters.
* The parameters in the file have the following meaning:
* <ul>
* <li> Width = width of the visible area (rectangular) in millimeters. Typically 300-310 mm.
* <li> Height = height of the visible area in millimeters. You must be sure that the ratio  Width/height is equal to the pixel aspect ratio of the screen. Typically 240 mm.
* <li> FocalDistance = the distance of the projected image (must be negative!)
* <li> OffsetX = the offset in mm needed to align the screen center w.r.t the (0,0,z) in the Optotrak system along the x axis. Typically some millimeters.
* <li> OffsetY = the offset in mm needed to align the screen center w.r.t the (0,0,z) in the Optotrak system along the y axis. Typically some millimeters.
* </ul>
* Once the screen is calibrated, the corresponding extremal points \f$ \mathbf{p}_a, \mathbf{p}_b, \mathbf{p}_c \f$ are generated, and you should pass
* the current \ref Screen instance to the \ref VRCamera current instance.
* For example:
* \code
* Screen screen;
* screen.loadSettings("calibration.txt");
* // Here we setup the camera
* VRCamera cam;
* // Camera is set up with correct parameters for alignment
* cam.init(screen);
* // Ok we are done to write the remaining code :P
* \endcode

A useful property of the Screen class is its ability to simulate a passive experiment: a typical situation of passive display is when the projection
screen continously follows and is frontoparallel to the observer's eye and is centered on his line of sight.
The Screen object can be moved in the space by mean of an Affine transformation (Eigen::Affine3d)
initProjectionScreen(0, headEyeCoords.getRigidStart().getFullTransformation()*Translation3d(Vector3d(0,0,focalDistance)-eyeCalibration));

\code
if ( passiveMode )
{
   screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
   screen.setOffset(alignmentX,alignmentY);
   screen.setFocalDistance(_focalDist);
         screen.transform(_transformation);
   cam.init(screen);
}
else
{
   screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
   screen.setOffset(alignmentX,alignmentY);
   screen.setFocalDistance(_focalDist);
   cam.init(screen);
}
\endcode
Remember that when a transformation is applied the points \f$ \mathbf{p}_a,\mathbf{p}_b,\mathbf{p}_c \f$ are definitively moved from their position so
if you want to apply the transform on the next frame you have to reset them to their starting position.

**/
class Screen
{
public:
   EIGEN_MAKE_ALIGNED_OPERATOR_NEW
   Screen();
   Screen(const Screen &other);
   Screen(double width, double height, double offsetX,double offsetY,double focalDistance, const Eigen::Affine3d &A );
   Screen(double width, double height, double offsetX,double offsetY,double focalDistance);
   void init(double _width, double _height, double _offsetX, double _offsetY, double _focalDistance);
   void init(const Eigen::Vector3d &pa,const Eigen::Vector3d &pb,const Eigen::Vector3d &pc,const Eigen::Vector3d &pd);
   void setWidthHeight(double width, double height);
   void setOffset(double x, double y);
   void setFocalDistance(double z);
   void translate(double x, double y, double z);
   void translate(const Eigen::Vector3d &t);
   void rotate( const Eigen::Matrix3d &);
   void transform(const Eigen::Affine3d &);
   Eigen::Vector3d getCenter() const;
   Screen getTransformed(const Eigen::Affine3d &A);
   void print(ostream &os);
   double screenWidth, screenHeight, focalDistance, offsetX, offsetY;
   Eigen::Vector3d pa,pb,pc,pd;

};
#endif
