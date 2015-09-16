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


#include "ShadedStimulus.h"

/**
* \ingroup Stimulus
* Default constructor
**/
ShadedStimulus::ShadedStimulus() : scaleX(1.0),scaleY(1.0),scaleZ(1.0)
{
}
/**
* \ingroup Stimulus
* \param other Shaded stimulus to copy
**/
ShadedStimulus::ShadedStimulus(const ShadedStimulus &other) :
    scaleX(other.scaleX),scaleY(other.scaleY),scaleZ(other.scaleZ)
{

}
/**
* \ingroup Stimulus
**/
ShadedStimulus::~ShadedStimulus()
{

}

/**
* \ingroup Stimulus
* \param os output stream to write on
**/
void ShadedStimulus::print(ostream &os)
{

}


/**
* \ingroup Stimulus
**/
void ShadedStimulus::compute()
{  cerr << "Implement your own!!! To do so, please follow the scheme of inheritance of SpherePointsStimulus.cpp for example!" << endl;
}

/**
* \ingroup Stimulus
* \param ax Scale on X axis
* \param ay Scale on Y axis
* \param az Scale on Z axis
**/
/*
void ShadedStimulus::setScale(double ax, double ay, double az)
{  scaleX=ax;
   scaleY=ay;
   scaleZ=az;
}
*/

