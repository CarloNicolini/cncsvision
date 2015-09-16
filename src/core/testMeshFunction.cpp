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
#include <Eigen/Dense>
#include <algorithm>
#include <complex>

#include "Mathcommon.h"
#include "MatrixStream.h"
#include "Convolution.h"
#include "Timer.h"

using namespace std;
using namespace Eigen;

double func(double x, double y)
{
    return exp( -(x*x+y*y));
}

int main(void)
{
    Vector2d xDomain(-M_PI*2,M_PI*2),yDomain(-M_PI*2,M_PI*2);

    MatrixXd Xmesh,Ymesh,Zmesh;
    double xstep=0.01, ystep=0.01;
    mathcommon::meshgrid<MatrixXd, double> (xDomain(0),xDomain(1),xstep,yDomain(0),yDomain(1),ystep,Xmesh,Ymesh);
    Zmesh.setZero(Xmesh.rows(),Xmesh.cols());

    #pragma omp parallel for
    for (int i=0; i<Xmesh.rows(); i++)
    {
    	for (int j=0; j<Xmesh.cols(); j++)
        {
            Zmesh(i,j)=func(Xmesh(i,j),Ymesh(i,j));
        }
    }

    cout << Zmesh << endl;

    return 0;
}
