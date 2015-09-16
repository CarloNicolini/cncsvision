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

#ifndef _CIRCLE_H_
#define _CIRCLE_H_
#include <set>

/** \ingroup Geometry
 *  A simple wrapper for a circle class in 2D, it handles collision with other circles, order comparison, a simple neighbors relation and is templatized
 */
template <typename T>
class Circle
{
public:
    Circle()
    {
        centerx=centery=0;
        radius=10;
    }

    Circle(T _centerx, T _centery, T _radius)
    {
        centerx=_centerx;
        centery=_centery;
        radius=_radius;
        active=true;
    }

    std::set<Circle<T> > neighbors;

    T centerx,centery,radius;
    bool addNeighbor( const Circle<T> &other)
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
    friend bool operator<(const Circle & a, const Circle & b)
    {
        if ( a.centerx == b.centerx)
            return a.centery < b.centery;
        else
            return a.centerx < b.centerx;
    }

    friend bool operator<=(const Circle & a, const Circle & b)
    {
        if ( a.centerx == b.centerx)
            return a.centery < b.centery;
        else
            return a.centerx <= b.centerx;
    }

    bool checkCollision(const Circle &other, T tolerance)
    {
        if ( ( SQR(centerx -other.centerx) + SQR(centery -other.centery) ) < SQR(radius+other.radius+tolerance) )
            return true;
        else
            return false;
    }
};

#endif
