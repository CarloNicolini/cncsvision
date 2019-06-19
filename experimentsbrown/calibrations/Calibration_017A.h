// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2011 Carlo Nicolini <carlo.nicolini@iit.it>
//
//
// CNCSVision is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//f
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

/*******  Define screen resolution, (width,height )in pixel and (widht, height )in millimeters *******/
#include <Eigen/Core>
#define TIMER_MS 13 // 75 hz
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
extern const double SCREEN_WIDE_SIZE = 306;    // millimeters

/********* CALIBRATION  This coordinate represents the origin (0.0, 0.0, 0.0) in the reference frame **/
static const Vector3d calibration(160,179,-75);
extern const Eigen::Vector3d frameOrigin=calibration;
// Alignment between optotrak z axis and screen z axis
double alignmentX =  23;//33.5; EVAN CHANGED THIS -- BLAME HIM
double alignmentY =  42.5;//33; THINGS WERE NOT ALIGNED
double homeFocalDistance=-270.0, focalDistance= homeFocalDistance, baseFocalDistance= homeFocalDistance;

// Optotrak alignment file , this is a relative path
extern char LastAlignedFile[] = "standard.cam";

// This is the width of chin rest in millimeters which is needed to calibrate the two eyes
static const double ChinRestWidth = 215.0;
static const char GAME_MODE_STRING[]="1024x768:32@75";

// Some Optotrak local variables for the Trieste setup. 
// See Optotrak,Optotrak2 classes to understand their meaning
static const int OPTO_NUM_MARKERS  = 16;
static const float OPTO_FRAMERATE = 75.0f;
static const float OPTO_MARKER_FREQ = 4600.0f;
static const float OPTO_DUTY_CYCLE = 0.4f;
static const float OPTO_VOLTAGE = 7.0f;

/***** matrices and vectors for calibration ****/

MatrixXd calibrationM(3,3), calibrationM_tr(3,3), referenceM(3,3), rotationM(3,3);
Vector3d origin_raw(0.0,0.0,0.0);

void initReferenceM()
{
	referenceM << -81.772, 2.64, -0.208,
			   0.158, 113.874, 3.595,
			   -0.331, -0.75, -82.19;
}