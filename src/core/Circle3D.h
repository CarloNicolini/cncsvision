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

#ifndef _Circle3D_H_
#define _Circle3D_H_
#include <set>

#ifndef SQR
#define SQR(x) x * x
#endif
/** \ingroup Geometry
 *  A simple wrapper for a Circle3D class in 2D, it handles collision with other Circle3Ds, order comparison, a simple neighbors relation and is templatized
 */
template <typename T>
class Circle3D
{
public:
    Circle3D()
    {
        centerx=centery=centerz=0;
        radius=10;
    }

    Circle3D(T _centerx, T _centery, T _centerz, T _radius)
    {
        centerx=_centerx;
        centery=_centery;
        centerz = _centerz;
        radius=_radius;
        active=true;
    }

    std::set<Circle3D<T> > neighbors;

    T centerx,centery,centerz,radius;
    bool addNeighbor( const Circle3D<T> &other)
    {
        if ( (neighbors.find(other)!=neighbors.end()) )
        {
            neighbors.insert(other);
            return true;
        }
        else
            return false;
    }

    bool active;
    friend bool operator<(const Circle3D & a, const Circle3D & b)
    {
        if ( a.centerx == b.centerx)
            return a.centery < b.centery;
        else
            return a.centerx < b.centerx;
    }

    friend bool operator<=(const Circle3D & a, const Circle3D & b)
    {
        if ( a.centerx == b.centerx)
            return a.centery < b.centery;
        else
            return a.centerx <= b.centerx;
    }

    bool checkCollision(const Circle3D &other, T tolerance)
    {
        if ( ( SQR(centerx -other.centerx) + SQR(centery -other.centery) + SQR(centerz -other.centerz )) < SQR(radius+other.radius+tolerance) )
            return true;
        else
            return false;
    }
};

#endif

