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

#ifndef _OPTICFLOW_ESTIMATOR_H_
#define _OPTICFLOW_ESTIMATOR_H_

#include <iostream>
#include <Eigen/Core>

#include "IncludeGL.h"

class OpticFlowEstimator
{
public:
   OpticFlowEstimator();
   ~OpticFlowEstimator();

   void init( int width, int height );
   void getPreBuffer();
   void getPostBuffer();
   void computeHS(double alpha);
   void computeEigenSolver();
   void printFlow( std::ostream &);
   void draw();
   void setAlpha(double);

   int width, height;

   Eigen::Matrix < GLfloat, Dynamic, Dynamic > frame1, frame2;
   Eigen::Matrix < GLfloat, Dynamic, Dynamic > frameDx, frameDy, frameDt;
   Eigen::Matrix < GLfloat, Dynamic, Dynamic > velX,velY,vModule,uAvg,vAvg,alphaOnes,ones,divisor,vAngle;

   float *colorDirection;
   Eigen::Matrix<GLfloat,2,2> Kx,Ky,Kt,KtOpposite;
   Eigen::Matrix<GLfloat,3,3> Ksmooth;
};

#endif
