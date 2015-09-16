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
#define TIMER_MS 12
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels

extern const double SCREEN_WIDE_SIZE = 383.0;    // millimeters

/********* CALIBRATION  This coordinate represents the origin (0.0, 0.0, 0.0) in the reference frame **/
// extern const Eigen::Vector3d calibration( 168.9, 347.9, 57.92 ); // valido per Aligned20121106_2.cam
extern const Eigen::Vector3d calibration( 150.93, 366.8, 57.92 );
extern const Eigen::Vector3d frameOrigin=calibration;

// Alignment between optotrak z axis and screen z axis
extern const double alignmentX =  9.25;//22.25 ;
extern const double alignmentY =  14.75;//22.5;
extern const double baseFocalDistance= -368.5;
// Optotrak alignment file , this is a relative path
char LastAlignedFile[] = "C:/cncsvisiondata/cameraFiles/Aligned20130524_5.cam";

// This is the width of chin rest in millimeters which is needed to calibrate the two eyes
static const double ChinRestWidth = 215.0;

static const char TS_GAME_MODE_STRING[]="1024x768:32@85";

// Some Optotrak local variables for the Trieste setup. 
// See Optotrak,Optotrak2 classes to understand their meaning
static const int TS_N_MARKERS  = 20;
static const float TS_FRAMERATE = 85.0f;
static const float TS_MARKER_FREQ = 3000.0f;
static const float TS_DUTY_CYCLE = 0.4f;
static const float TS_VOLTAGE = 8.0f;
