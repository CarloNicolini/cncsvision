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

#include <iostream>
#include <fstream>
#include <string>

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

#include "Helicoid.h"

Helicoid::Helicoid(double _innerRadius, double _outerRadius, double _startTheta, double _endTheta, int _nStepsRadius, int _nStepsTheta)
{
    this->innerRadius=_innerRadius;
    this->outerRadius=_outerRadius;
    this->startTheta = _startTheta;
    this->endTheta = _endTheta;
    this->nStepsRadius = _nStepsRadius;
    this->nStepsTheta=_nStepsTheta;

    double deltaR = std::abs(_outerRadius-_innerRadius)/_nStepsRadius;
    double deltaTheta = std::abs(_endTheta-_startTheta)/_nStepsTheta;
    for (double r=_innerRadius; r<=_outerRadius; r+=deltaR)
    {
        for  (double theta=_startTheta; theta<=_endTheta; theta+=deltaTheta)
        {
            coordinates.push_back( getHelicoidCoordinates(r,theta) );
            //normals.push_back(getHelicoidNormal(r,theta));
        }
    }
}

void Helicoid::rotate(const Eigen::AngleAxis<double> &aa)
{
    std::vector<Eigen::Vector3d> tmp = coordinates;
    for (int i=0; i<coordinates.size();i++)
    {
        tmp.at(i) <<  aa.toRotationMatrix()*coordinates.at(i);
    }
    coordinates=tmp;
}

Eigen::Vector3d Helicoid::getHelicoidCoordinates(double r, double theta)
{
    return Eigen::Vector3d( r*cos(theta),theta,r*sin(theta));
}

Eigen::Vector3d Helicoid::getHelicoidTangent(double r, double theta)
{
    return Eigen::Vector3d( cos(theta),0,sin(theta));
}

Eigen::Vector3d Helicoid::getHelicoidBitangent(double r, double theta)
{
    return Eigen::Vector3d( -r*sin(theta),1,r*cos(theta) );
}
Eigen::Vector3d Helicoid::getHelicoidNormal(double r, double theta)
{
    return  getHelicoidTangent(r,theta).cross(getHelicoidBitangent(r,theta)).normalized();
}

void Helicoid::draw()
{
    double *pCoords = &(this->coordinates.at(0).coeffRef(0));
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_DOUBLE, 0,pCoords);
    glDrawArrays(GL_POINTS, 0, coordinates.size());
    glDisableClientState(GL_VERTEX_ARRAY);
}


