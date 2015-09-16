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

#ifndef _TRIANGLE3_H_
#define _TRIANGLE3_H_

#include <iostream>
#include <Eigen/Core>

using Eigen::Vector3d;

/**
 * @brief The Triangle3 struct
 */
struct Triangle3
{
    Triangle3(Vector3d p0, Vector3d p1, Vector3d p2) : p0(p0), p1(p1), p2(p2){}

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

    Vector3d p0,p1,p2;
    /**
     * @brief area
     * @return
     */
    double area()
    {
        return (p0-p1).cross((p0-p2)).norm()/2;
    }

    // http://math.stackexchange.com/questions/4322/check-whether-a-point-is-within-a-3d-triangle
    /**
     * @brief contains
     * @param x
     * @return
     */
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

    // http://math.stackexchange.com/questions/544946/determine-if-projection-of-3d-point-onto-plane-is-within-a-triangle
    /**
     * @brief contains_projection
     * @param x
     * @return
     */
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

    /**
     * @brief projected
     * @param x
     * @return
     */
    Vector3d projected(const Vector3d &x)
    {
        return Hyperplane<double,3>::Through(p0,p1,p2).projection(x);
    }
};

/**
 * @brief operator <<
 * @param os
 * @param dt
 * @return
 */
inline ostream& operator<<(ostream& os, const Triangle3& dt)
{
    os << dt.p0.transpose() << " ; " << dt.p1.transpose() << " ; " << dt.p2.transpose() << endl;
    return os;
}


/**
 * @brief triangulate3
 * @param p0
 * @param p1
 * @param p2
 * @param p3
 * @return
 */
inline pair<Triangle3,Triangle3> triangulate3(const Vector3d &p0, const Vector3d &p1, const Vector3d &p2, const Vector3d &p3)
{
    Triangle3 t0(p0,p1,p2),t1(p0,p2,p3);
    return pair<Triangle3,Triangle3>(t0,t1);
}

#endif
