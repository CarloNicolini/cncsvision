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
#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace Eigen;
using namespace std;

Vector3d getRayCylinderIntersection( const Vector3d &rayDirection, const Vector3d rayOrigin, float cylinderRadius )
{
    double a = rayDirection.x()*rayDirection.x()+rayDirection.y()*rayDirection.y();
    double b = 2*(rayDirection.x()*rayOrigin.x()+rayOrigin.y()*rayDirection.y());
    double c = rayOrigin.x()*rayOrigin.x()+rayOrigin.y()*rayOrigin.y()-cylinderRadius*cylinderRadius;
    
    if (b*b-4*a*c<0)
        cerr << "no intersection" << endl;
    
    double t = (-b+sqrt(b*b-4*a*c))/(2*a);
    
    return rayDirection*t+rayOrigin;
}

/*
 *return -(hyperplane.offset()+hyperplane.normal().dot(origin()))
          / hyperplane.normal().dot(direction());
}
 */
Vector3d getRayPlaneIntersection(Vector3d rayDirection, Vector3d rayOrigin, Vector3d mirrorNormal, Vector3d mirrorPlaneOrigin )
{
    double offset = -mirrorNormal.dot(mirrorPlaneOrigin);
    double t = -(offset+mirrorNormal.dot(rayOrigin))/ mirrorNormal.dot(rayDirection);
    return rayDirection*t+rayOrigin;
}

//Eigen::Hyperplane<double,3> focalPlane = Eigen::Hyperplane<double,3>::Through( Vector3d(1,0,0), Vector3d(0,1,0),center );

int main()
{
    Vector3d n1(0,1,1);
    Vector3d n2(0,0,0);

    // I initialize the line with two points passing through it
    ParametrizedLine<double,3> pline = ParametrizedLine<double,3>::Through(n1,n2);
    cout <<  pline.direction().transpose() << " \t " << pline.origin().transpose() << endl;

    Vector3d planeNormal(0,0,-1);
    Vector3d aPointOnThePlane(0,0,-1);

    // Here is my plane, it passes through 3 points
    Hyperplane<double,3> focalPlane(planeNormal,aPointOnThePlane);

    // So if I'm correct this is the t at which pline intersect the last plane "focalPlane"
    double intersectionParam = pline.intersection( focalPlane ) ;
    cerr << "Intersection parameter = " << intersectionParam << endl;
    Vector3d intersection = intersectionParam*(n2-n1).normalized()+n1;

    // Now, how to compute the coordinates of such intersection? I think this is the correct way...
    cerr << "EIGEN METHOD " << intersection.transpose()  << endl;
    cerr << "MY METHOD " << getRayPlaneIntersection(n2-n1,n1,planeNormal,aPointOnThePlane).transpose() << endl;

    return 0;
}




