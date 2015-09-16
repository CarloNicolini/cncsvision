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

#ifndef _STIMULUS_DRAWER_
#define _STIMULUS_DRAWER_

#include <Eigen/Core>
#include <vector>
#include "Point3D.h"
#include "Stimulus.h"
#include "PointwiseStimulus.h"
#include "SpherePointsStimulus.h"
#include "CubePointsStimulus.h"
#include "CylinderPointsStimulus.h"
#include "BoxNoiseStimulus.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "Screen.h"

#include "IncludeGL.h"
/**
* \class StimulusDrawer
* \ingroup GLVisualization
* \brief Handles a stimulus and draw it in the OpenGL window.
* It uses the call lists to load the stimulus once and draw it faster by keeping it in the GPU memory
**/
class StimulusDrawer
{
public:
    StimulusDrawer();
    ~StimulusDrawer();
    void setStimulus(Stimulus *);
    void initList(Stimulus *s, const GLfloat color[4]=glRed, GLfloat pointSize=2.0f );
    void setSpheres(bool );
    void setRadius( double r);
    void drawSpecialPoints();
    void draw();
    vector<Eigen::Vector3d> projectStimulusPoints(const Affine3d &objectTransformation, const Affine3d &activeHeadTransformation, const VRCamera &cam, double focalDistance, const Screen &screen, const Vector3d &eyeCalibration, bool passiveMode, bool draw );
    MatrixXd computeOpticFlow(const vector<Vector3d> &projectStimulusPoints,double focalDistance,double deltaTInSeconds );
    Eigen::Vector3d getProjectedStimulusCenter();

private:
    Stimulus *currentStimulus;
    vector <Stimulus*> stimuli;

	Vector3d alpha,beta;
    bool drawSpheres,drawSpecial;
    GLuint stimListIndex;
    double radius;
    double gx;
    double gy;
};

#endif
