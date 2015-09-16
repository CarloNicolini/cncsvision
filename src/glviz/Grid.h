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

#ifndef _GRID_H_
#define _GRID_H_

#include <Eigen/Core>
#include <Eigen/QR>
#include <Eigen/Geometry>

#include "IncludeGL.h"
/**
*
* \class Grid
* \ingroup GLVisualization
* \brief This class is needed to draw OpenGL grids simply using evaluators. To start a 2nd order bezier surface (grid) you must simply
* specify the coordinates of the edges and the number of subdivisions.
*
* For example, we want to draw a 1 cm x 1 cm grid with 10 squares per edge.
* First make sure that a proper OpenGL environment is started (usually use this class after the glutInit )
* \code
* Grid grid;
* grid.setRowsAndCols(10,10); //here we set the number of columns and rows per edge
* // here we measure in
* grid.init(Vector3d(-0.5, -0.5, 0.0),Vector3d(0.5, -0.5, 0.0),Vector3d(-0.5, 0.5, 0.0),Vector3d(0.5, 0.5, 0.0));
* // then some other code, and when you want to draw:
* grid.draw();
* \endcode
*
**/

class Grid
{
public:
   Grid();
   ~Grid();
   void init(double xMin, double xMax, double yMin, double yMax);
   void init(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2, const Eigen::Vector3d &p3, const Eigen::Vector3d &p4 );
   void init(const Eigen::Hyperplane<double,3> &plane, double edgeLength=100);
   void setRowsAndCols(int stepsX, int stepsY);

   void draw(bool fill=false,double alpha=1.0);

private:
   int stepsX,stepsY;
   GLfloat ctrlpoints[2][2][3];
};

#endif
