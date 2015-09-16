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

#include "ExperimentalParameters.h"

using namespace Eigen;

ExperimentalParameters::ExperimentalParameters()
{
    this->projCenter = Vector3d(0,250,0);
    this->reflectedProjector = Vector3d(250,0,0);

    this->projectorPitch=90.0;
    this->projectorRoll=90.0;

    this->projDirection <<  sin(projectorPitch*M_PI/180)*cos(projectorRoll*M_PI/180),
            sin(projectorPitch*M_PI/180)*sin(projectorRoll*M_PI/180),
                        cos(projectorPitch*M_PI/180);

    this->projectorFocal = 150.0; //focale in millimetri
    this->projectorAreaEdge=100;
    this->projectorApertureDegrees=30;
    this->viewerDistance=200;
    this->viewerHeight=0.0;
    this->cylinderHeight=250;

    this->meshVertices = 0;

    this->mirrorTilt=M_PI/4.0;
    this->mirrorRotationDegree=0.0;
    this->mirrorPlane = Eigen::Hyperplane<double,3>(Vector3d(0,0,1),0);
    this->mirrorCenter = Vector3d::Zero();

    this->perspectiveView=true;
    this->mirrorAngularSpeedDegForSec=0.0;
    this->mirrorEdgeLenght=50;

    this->viewerAngle = 0.0; //radians

    this->objectScale=100;
    this->totalViewPoints = 96;
    this->currentViewIndex=0; // index of current view
}
