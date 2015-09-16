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
#include "GeometricFunctions.h"
using std::cerr;
using std::endl;

namespace GeometricFunctions
{
    // Assumes cylinder aligned on Y axis!!!
    Eigen::Vector3d getRayCylinderIntersection( const Eigen::Vector3d &rayDirection, const Eigen::Vector3d &rayOrigin, float cylinderRadius )
    {
        double a = rayDirection.x()*rayDirection.x()+rayDirection.z()*rayDirection.z();
        double b = 2*(rayDirection.x()*rayOrigin.x()+rayOrigin.z()*rayDirection.z());
        double c = rayOrigin.x()*rayOrigin.x()+rayOrigin.z()*rayOrigin.z()-cylinderRadius*cylinderRadius;
        double t = (-b+sqrt(b*b-4*a*c))/(2*a);

        return rayDirection*t+rayOrigin;
    }

    Eigen::Vector3d getRayPlaneIntersection(const Eigen::Vector3d &rayDirection, const Eigen::Vector3d &rayOrigin, const Eigen::Vector3d &planeNormal, const Eigen::Vector3d &planeOrigin)
    {
        double offset = -planeNormal.dot(planeOrigin);
        double t = -(offset+planeNormal.dot(rayOrigin))/ planeNormal.dot(rayDirection);
        return rayDirection*t+rayOrigin;
    }

    Eigen::Vector3d getReflectedRay(const Eigen::Vector3d &rayDirection, const Eigen::Vector3d &normal)
    {
        //For a given incident vector I and surface normal N reflect returns
        //the reflection direction calculated as I - 2.0 * dot(N, I) * N.
        Eigen::Vector3d n = normal.normalized();
        return rayDirection-2*rayDirection.dot(n)*n;
    }

    Eigen::Vector2d project(const Eigen::Vector3d &p)
    {
        GLint viewport[4]={0,0,0,0};
        GLdouble modelview[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        GLdouble projection[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        glGetIntegerv(GL_VIEWPORT, viewport);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        double x1=0.0,y1=0.0,z1=0.0;
        if(gluProject(p.x(),p.y(),p.z(),modelview,projection,viewport,&x1,&y1,&z1) != GL_TRUE)
        {  //cerr << "error in gluProject at GLUtils::project\n";
            //exit(0);
        }

        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        y1 = viewport[3] - y1;

        return Eigen::Vector2d(x1,y1);
    }

    Eigen::Vector2d project(const Eigen::Vector3d &point, const Eigen::Projective3d &proj, const Eigen::Affine3d &modelview, const Eigen::Vector4i &viewport)
    {
        Eigen::Vector3d projected = ( proj*(modelview*point).homogeneous() ).eval().hnormalized();
        return Eigen::Vector2d(viewport(0) + viewport(2)*(projected.x()+1)/2,viewport(1) + viewport(3)*(projected.y()+1)/2);
    }
}
