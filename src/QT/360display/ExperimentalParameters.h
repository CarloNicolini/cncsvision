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

#ifndef EXPERIMENTAL_PARAMETERS_H
#define EXPERIMENTAL_PARAMETERS_H

#include <Eigen/Core>
#include <Eigen/Geometry>
#include "Screen.h"

class ExperimentalParameters
{
public:
    ExperimentalParameters();

    Eigen::Vector3d projCenter, projDirection,reflectedProjector;

    double projectorApertureDegrees;

    double projectorPitch, projectorRoll;
    double projectorFocal;//focale del proiettore in millimetri
    double projectorAreaEdge;

    double mirrorTilt;
    double mirrorRotationDegree;
    double mirrorAngularSpeedDegForSec;

    long meshVertices;

    double mirrorEdgeLenght;

    double viewerAngle;
    bool perspectiveView;
    Eigen::Hyperplane<double,3> mirrorPlane;
    Eigen::Vector3d mirrorCenter;

    double objectScale;
    double viewerDistance,viewerHeight;
    double cylinderHeight;

    int totalViewPoints;
    int currentViewIndex;
};

#endif
