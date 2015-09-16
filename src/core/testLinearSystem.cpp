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
#include <vector>
#include <cmath>
#include <deque>
#include <algorithm>
#include <iomanip>

#include <Eigen/Core>
#include <Eigen/QR>
#include <Eigen/Geometry>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include "Mathcommon.h"
#include "RigidBody.h"
#include "EulerExtractor.h"
#include "Timer.h"
#include "CoordinatesExtractor.h"
#include "Screen.h"
#include "MatrixStream.h"
#include "Util.h"
#include "Point3D.h"

using namespace std;
using namespace mathcommon;
using namespace Eigen;

#include "Screen.h"

struct Triangle
{
    inline Vector2d operator[](size_t i)
    {
        switch (i)
        {
        case 0:
            return p0;
            break;
        case 1:
            return p1;
            break;
        case 2:
            return p2;
            break;
        }
    }
    inline Triangle operator+(const Vector2d &d)
    {
        return Triangle(p0+d,p1+d,p2+d);
    }
    inline Triangle operator-(const Vector2d &d)
    {
        return Triangle(p0-d,p1-d,p2-d);
    }

    Triangle(Vector2d p0, Vector2d p1, Vector2d p2) : p0(p0), p1(p1), p2(p2){}
    Vector2d p0,p1,p2;

    double area()
    {
        return abs(1.0/2.0*(-p1.y()*p2.x() + p0.y()*(-p1.x() + p2.x()) + p0.x()*(p1.y() - p2.y()) + p1.x()*p2.y()));
    }
};


struct Triangle3
{
    inline Vector3d operator[](size_t i)
    {
        switch (i)
        {
        case 0:
            return p0;
            break;
        case 1:
            return p1;
            break;
        case 2:
            return p2;
            break;
        }
    }
    inline Triangle3 operator+(const Vector3d &d)
    {
        return Triangle3(p0+d,p1+d,p2+d);
    }
    inline Triangle3 operator-(const Vector3d &d)
    {
        return Triangle3(p0-d,p1-d,p2-d);
    }

    inline void operator -=(const Vector3d &d)
    {
        p0-=d;
        p1-=d;
        p2-=d;
    }

    inline void operator +=(const Vector3d &d)
    {
        p0+=d;
        p1+=d;
        p2+=d;
    }

    Triangle3(Vector3d p0, Vector3d p1, Vector3d p2) : p0(p0), p1(p1), p2(p2){}
    Vector3d p0,p1,p2;
    double area()
    {
        return (p0-p1).cross((p0-p2)).norm()/2;
    }
    // http://math.stackexchange.com/questions/4322/check-whether-a-point-is-within-a-3d-triangle
    bool contains(const Vector3d &x)
    {
        double A = this->area();
        double alpha = (x-p1).cross(x-p2).norm()/(2*A);
        double beta = (x-p2).cross(x-p0).norm()/(2*A);
        double gamma = 1-alpha-gamma;

        bool ba = 0<=alpha && alpha<=1;
        bool bb = 0<=beta && beta<=1;
        bool bg = 0<=gamma && gamma<=1;

        if (ba && bb && bg)
            return true;
        else
            return false;
    }

    // http://math.stackexchange.com/questions/4322/check-whether-a-point-is-within-a-3d-triangle
    bool contains_projection(const Vector3d &x)
    {
        Vector3d u = p1-p0;
        Vector3d v = p2-p0;
        Vector3d n = u.cross(v);
        Vector3d w = x-p0;
        double gamma = (u.cross(w).dot(n))/n.dot(n);
        double beta = (w.cross(v).dot(n))/n.dot(n);
        double alpha = 1-gamma-beta;

        bool ba = 0<=alpha && alpha<=1;
        bool bb = 0<=beta && beta<=1;
        bool bg = 0<=gamma && gamma<=1;

        if (ba && bb && bg)
            return true;
        else
            return false;
    }
    Vector3d projected(const Vector3d &x)
    {
        return Hyperplane<double,3>::Through(p0,p1,p2).projection(x);
    }
};

ostream& operator<<(ostream& os, const Triangle3& dt)
{
    os << dt.p0.transpose() << " ; " << dt.p1.transpose() << " ; " << dt.p2.transpose() << endl;
    return os;
}

/**
 * @brief isInTriangle check if a point is inside a triangle:
 * http://stackoverflow.com/questions/2049582/how-to-determine-a-point-in-a-triangle
 * @param pp
 * @param p1
 * @param p2
 * @param p3
 * @return
 */
bool isInTriangle(const Vector2d &p, const Eigen::Vector2d &p0,const Eigen::Vector2d &p1,const Eigen::Vector2d &p2)
{
    double Area = 1.0/2.0*(-p1.y()*p2.x() + p0.y()*(-p1.x() + p2.x()) + p0.x()*(p1.y() - p2.y()) + p1.x()*p2.y());
    double s = 1.0/(2*Area)*(p0.y()*p2.x() - p0.x()*p2.y() + (p2.y() - p0.y())*p.x() + (p0.x() - p2.x())*p.y());
    double t = 1.0/(2*Area)*(p0.x()*p1.y() - p0.y()*p1.x() + (p0.y() - p1.y())*p.x() + (p1.x() - p0.x())*p.y());

    if (s >0 && t>0 && ((1-s-t) >0) )
        return true;
    else
        return false;
}

/**
 * @brief isInTriangle
 * @param p
 * @param tri
 * @return
 */
bool isInTriangle(const Vector2d &p, const Triangle &tri)
{
    return isInTriangle(p,tri.p0,tri.p1,tri.p2);
}

/**
 * @brief triangulate
 * @param p0
 * @param p1
 * @param p2
 * @param p3
 * @return
 */
pair<Triangle,Triangle> triangulate(Vector2d p0, Vector2d p1, Vector2d p2, Vector2d p3)
{
    Triangle t0(p0,p1,p2),t1(p0,p2,p3);
    return pair<Triangle,Triangle>(t0,t1);
}

/**
 * @brief triangulate
 * @param p0
 * @param p1
 * @param p2
 * @param p3
 * @return
 */
pair<Triangle3,Triangle3> triangulate3(Vector3d p0, Vector3d p1, Vector3d p2, Vector3d p3)
{
    Triangle3 t0(p0,p1,p2),t1(p0,p2,p3);
    return pair<Triangle3,Triangle3>(t0,t1);
}

/**
 * @brief pointsInTrapeze
 * @param base1
 * @param base2
 * @param height
 */
void pointsInQuadrilateral()
{
    // Define the vertices of the trapezed
    Vector2d ll,lr,ul,ur;
    ll << -5,-3;
    lr << 4,-1;
    ul << -3,3;
    ur << 3,3;

    pair<Triangle,Triangle> tri(Triangle(ll,lr,ur),Triangle(ur,ul,ll));
    double w0 = tri.first.area()/(tri.first.area()+tri.second.area());
    double w1 = tri.second.area()/(tri.first.area()+tri.second.area());

    for (int i=0; i< 100000; ++i)
    {
        Vector2d p = Vector2d::Random()*10;
        if (isInTriangle(p,tri.first) )
            cout << p.transpose() << endl;
        if (isInTriangle(p,tri.second) )
            cout << p.transpose() << endl;
    }
}

/**
 * @brief pointsInQuadrilateral2
 */
void pointsInQuadrilateral2()
{
    // Define the vertices of the trapezed
    Vector3d P0,P1,P2,P3;
    P0 << -5,-1,0;
    P1 << 5,-1,0;
    P2 << 2,1,10;
    P3 << -2,1,10;

    vector<Vector3d> pp;
    pp.push_back(P0);
    pp.push_back(P1);
    pp.push_back(P2);
    pp.push_back(P3);

    double xmax=std::numeric_limits<double>::min();
    double xmin=std::numeric_limits<double>::max();
    double ymax=std::numeric_limits<double>::min();
    double ymin=std::numeric_limits<double>::max();
    double zmin=std::numeric_limits<double>::max();
    double zmax=std::numeric_limits<double>::min();

    for(int i=0; i<4;i++)
    {
        xmax = std::max(pp.at(i).x(),xmax);
        ymax = std::max(pp.at(i).y(),ymax);
        zmax = std::max(pp.at(i).z(),zmax);

        xmin = std::min(pp.at(i).x(),xmin);
        ymin = std::min(pp.at(i).y(),ymin);
        zmin = std::min(pp.at(i).z(),zmin);
    }


    pair<Triangle3,Triangle3> tri = triangulate3(P0,P1,P2,P3);
    int i=0;
    while (i<1000)
    {
        Vector3d p = Vector3d::Random(); // [-1,1] x [-1,1] x [-1,1] random range
        p.x() *= (xmax-xmin)+xmin;
        p.y() *= (ymax-ymin)+ymin;
        p.z() *= (zmax-zmin)+zmin;
        if ( tri.first.contains_projection(p))
        {
            cout << tri.first.projected(p).transpose() << endl;
            ++i;
        }
        if (tri.second.contains_projection(p))
        {
            cout << tri.second.projected(p).transpose() << endl;
            ++i;
        }
    }
}

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    srand(time(0));
    pointsInQuadrilateral2();
    return 0;
}
