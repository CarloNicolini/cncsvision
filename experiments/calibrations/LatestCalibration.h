// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2013 Carlo Nicolini <carlo.nicolini@iit.it>
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
#define TIMER_MS 10
#define SCREEN_WIDTH  1024      // pixels
#define SCREEN_HEIGHT 768       // pixels
#define SCREEN_REFRESH_RATE 100.0

extern const double SCREEN_WIDE_SIZE = 310.0;    // millimeters
extern double focalDistance = -380.5;
extern const double homeFocalDistance = -380.5;
// SCREEN_HEIGHT_SIZE is  SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH to mantain proportions

extern const Eigen::Vector3d calibration( -420.0, 502.0, 390.0 ); //new relative to calibration 28 june 2016
extern const Eigen::Vector3d frameOrigin=calibration;

// Alignment between optotrak z axis and screen z axis
// Se si fa la calibrazione del monitor cambierà sicuramente anche alignmentX
extern const double alignmentX =  12.50 ; //new relative to calibration 28 june 2016
extern const double alignmentY =  25.75; //new relative to calibration 28 june 2016

#ifdef WIN32
char LastAlignedFile[] = "C:/cncsvisiondata/cameraFiles/LatestCameraAlignment.cam";
#endif

#ifdef __linux__
char LastAlignedFile[] = "latest_camera_alignment.cam";
#endif

static const char ROVERETO_GAME_MODE_STRING[]="1024x768:32@100";
static const char GAME_MODE_STRING[]="1024x768:32@100";

static const int OPTO_NUM_MARKERS  = 22;
static const float OPTO_FRAMERATE = 100.0f;
static const float OPTO_MARKER_FREQ = 4600.0f;
static const float OPTO_DUTY_CYCLE = 0.66f;
static const float OPTO_VOLTAGE = 10.0f;
