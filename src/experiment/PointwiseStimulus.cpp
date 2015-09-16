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
#include <map>
#include <Eigen/Core>

#include "Point3D.h"
#include "PointwiseStimulus.h"

/**
* \ingroup Stimulus
* Default constructor
* Set the following parameters to their defaults: fluffiness=1e-6
**/
PointwiseStimulus::PointwiseStimulus() :fluffiness(1e-6)
{
}
/**
* \ingroup Stimulus
* Copy constructor, copy a given stimulus
**/
PointwiseStimulus::PointwiseStimulus(const PointwiseStimulus &other) : pointsRand(other.pointsRand), fluffiness(other.fluffiness), specialPointsRand(other.specialPointsRand)
{
}
/**
* \ingroup Stimulus
**/
PointwiseStimulus::~PointwiseStimulus()
{
    for(PointsRandIterator iPoint = pointsRand.begin(); iPoint != pointsRand.end(); ++iPoint )
    {  delete *iPoint;
    }

    for(PointsRandIterator iPoint = specialPointsRand.begin(); iPoint != specialPointsRand.end(); ++iPoint )
    {  delete *iPoint;
    }

}
/**
* \ingroup Stimulus
* Set the total number of points, this call must precede compute()
* \param _n Total number of points composing the stimulus
**/
void PointwiseStimulus::setNpoints(unsigned int _n)
{
    if (this->pointsRand.size() <= _n)
    {
        while ( this->pointsRand.size() < _n )
            this->addPoint();
    }
    else
    {
        while ( this->pointsRand.size() > _n )
        {
            delete pointsRand.back();
            pointsRand.pop_back();
        }
     }

}
/**
* \ingroup Stimulus
* Add a point to the list
**/
void PointwiseStimulus::addPoint( void )
{

    Point3D *p = new Point3D();
    pointsRand.push_back(p);

}

/**
* \ingroup Stimulus
* Shuffle randomly the points in a [-1,1]x[-1,1]x[-1,1] interval
**/
void PointwiseStimulus::shuffle()
{  for(PointsRandIterator iPoint = pointsRand.begin(); iPoint != pointsRand.end(); ++iPoint )
    {  (*iPoint)->x = mathcommon::unifRand(-1.,1.);
        (*iPoint)->y = mathcommon::unifRand(-1.,1.);
        (*iPoint)->z = mathcommon::unifRand(-1.,1.);
    }
}
/**
* \ingroup Stimulus
* \param os output stream to write on
**/
void PointwiseStimulus::print(ostream &os)
{  for(PointsRandIterator iPoint = pointsRand.begin(); iPoint != pointsRand.end(); ++iPoint )
    os << *(*iPoint) << endl;

}

/**
* \ingroup Stimulus
* \param _fluf Fluffiness of the stimulus
**/
void PointwiseStimulus::setFluffiness(double _fluf)
{  fluffiness = _fluf;
}

/**
* \ingroup Stimulus
**/
void PointwiseStimulus::compute()
{  std::cerr << "Implement your own!!! To do so, please follow the scheme of inheritance of SpherePointsStimulus.cpp for example!" << endl;

}


