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
#include <Eigen/Core>


#include "Point3D.h"

#include "BoxNoiseStimulus.h"
#include "CubePointsStimulus.h"
#include "CylinderPointsStimulus.h"
#include "PointwiseStimulus.h"
#include "SpherePointsStimulus.h"
#include "Stimulus.h"
#include "ExtrudedTrapezePointsStimulus.h"

using namespace std;
using namespace Eigen;

#define NPOINTS 1000

int main(int argc, char *argv[])
{
    ExtrudedTrapezePointsStimulus extr;
    extr.setHeight(2.0);
    extr.setWFar(5.0);
    extr.setWNear(1.0);
    extr.setZFar(1);
    extr.setZNear(0);
    extr.setDensity(20);
    extr.compute();
    extr.print(cout);
   return 0;
}
