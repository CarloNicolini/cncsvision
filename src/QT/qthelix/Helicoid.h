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

#ifndef _HELICOID_H_
#define _HELICOID_H_

#include <iostream>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Geometry>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif
#ifdef __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#endif

class Helicoid
{
public:
    Helicoid(double _innerRadius, double _outerRadius, double _startTheta, double _endTheta, int _nStepsRadius, int _nStepsTheta);
    void rotate(const Eigen::AngleAxis<double> &aa);
    void draw();

    Eigen::Vector3d getHelicoidCoordinates(double r, double theta);
    Eigen::Vector3d getHelicoidTangent(double r, double theta);
    Eigen::Vector3d getHelicoidBitangent(double r, double theta);
    Eigen::Vector3d getHelicoidNormal(double r, double theta);

    double innerRadius,outerRadius,startTheta,endTheta;
    int nStepsRadius, nStepsTheta;
    std::vector<Eigen::Vector3d> coordinates,normals;
};

#endif
