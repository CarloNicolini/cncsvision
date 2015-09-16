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
#include "Grid.h"

using namespace Eigen;
using namespace std;
/** \ingroup GLVisualization
 * Default constructor
**/
Grid::Grid()
{  stepsX=stepsY=5;
}

/** \ingroup GLVisualization
* Init the 4 corners of the grid
* @param p1 Lower left corner
* @param p2 Lower right corner
* @param p3 Top left corner
* @param p4 Top right corner
 **/
void Grid::init(const Vector3d &p1, const Vector3d &p2, const Vector3d &p3, const Vector3d &p4 )
{  ctrlpoints[0][0][0]=p1.x();
    ctrlpoints[0][0][1]=p1.y();
    ctrlpoints[0][0][2]=p1.z();

    ctrlpoints[0][1][0]=p2.x();
    ctrlpoints[0][1][1]=p2.y();
    ctrlpoints[0][1][2]=p2.z();

    ctrlpoints[1][0][0]=p3.x();
    ctrlpoints[1][0][1]=p3.y();
    ctrlpoints[1][0][2]=p3.z();

    ctrlpoints[1][1][0]=p4.x();
    ctrlpoints[1][1][1]=p4.y();
    ctrlpoints[1][1][2]=p4.z();

}

/** \ingroup GLVisualization
* Init the 4 corners of the grid from a Hyperplane equation representing object
* @param p1 Lower left corner
* @param p2 Lower right corner
* @param p3 Top left corner
* @param p4 Top right corner
 **/
void Grid::init(const Eigen::Hyperplane<double,3> &plane, double l)
{
    Vector3d a(-l,0,-l),b(-l,0,l),c(l,0,-l),d(l,0,l);

    //init(Vector3d(-edgeLength,-edgeLength,0),Vector3d(-edgeLength,edgeLength,0),Vector3d(edgeLength,-edgeLength,0),Vector3d(edgeLength,-edgeLength,0));
    AngleAxis<double> rot( acos( plane.normal().z() ), plane.normal() );
    Affine3d aff = Affine3d::Identity();
    aff.linear() = rot.toRotationMatrix();
    aff.translation() = plane.normal()*plane.offset();

    a = aff*a;
    b = aff*b;
    c = aff*c;
    d = aff*d;
    init(a,b,c,d);
}

void Grid::init(double xMin, double xMax, double yMin, double yMax)
{
    ctrlpoints[0][0][0]=xMin;
    ctrlpoints[0][0][1]=yMin;
    ctrlpoints[0][0][2]=0;

    ctrlpoints[0][1][0]=xMax;
    ctrlpoints[0][1][1]=yMin;
    ctrlpoints[0][1][2]=0;

    ctrlpoints[1][0][0]=xMin;
    ctrlpoints[1][0][1]=yMax;
    ctrlpoints[1][0][2]=0;

    ctrlpoints[1][1][0]=xMax;
    ctrlpoints[1][1][1]=yMax;
    ctrlpoints[1][1][2]=0;
}

/** \ingroup GLVisualization
 * Set number of rows and columns of the grid
 * @param _x Number of columns
 * @param _y Number of rows
**/
void Grid::setRowsAndCols(int _x, int _y)
{  stepsX=_x;
    stepsY=_y;
}

/** \ingroup GLVisualization
*   Draw the specified grid. A rendering context must be active!
**/
void Grid::draw(bool fill, double alpha)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_COLOR_MATERIAL);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);

    glEnable(GL_MAP2_VERTEX_3);
    glEnable(GL_AUTO_NORMAL);

    glMap2f(GL_MAP2_VERTEX_3,
            0.0, 1.0,  /* U ranges 0..1 */
            3,         /* U stride, 3 floats per coord */
            2,         /* U is 2nd order, ie. linear */
            0.0, 1.0,  /* V ranges 0..1 */
            2 * 3,     /* V stride, row is 2 coords, 3 floats per coord */
            2,         /* V is 2nd order, ie linear */
            &ctrlpoints[0][0][0]);  /* control points */

    glMapGrid2f(
                stepsX, 0.0, 1.0,
                stepsY, 0.0, 1.0);
    GLfloat color[4];
    if ( fill )
    {
        glGetFloatv(GL_CURRENT_COLOR,color);
        glEnable (GL_BLEND);
        glEnable(GL_COLOR_MATERIAL);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(color[0],color[1],color[2],alpha);
    }
    glEvalMesh2(fill ? GL_FILL : GL_LINE,
                0, stepsX,   /* Starting at 0 mesh 5 steps (rows). */
                0, stepsY);  /* Starting at 0 mesh 6 steps (columns). */


    glDisable(GL_MAP2_VERTEX_3);
    glDisable(GL_AUTO_NORMAL);


    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glPopAttrib();
}

/** \ingroup GLVisualization
 * Default destructor
**/
Grid::~Grid()
{

}
