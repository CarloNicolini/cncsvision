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


#ifndef _GL2DPainter_H_
#define _GL2DPainter_H_
#include "IncludeGL.h"
#include <iostream>
#include <string>

using namespace std;

/**
 * \ingroup GLVisualization
 * \class GL2DPainter
 * \brief GL2DPainter is a wrapper around the GLUT calls to font rendering, made simple for use.
 *
 * GL2DPainter allows you to specify only the screen size and the string you want to render and the text is rendered of the color you want and quite fast.
 * An example is the following function that reads from an input file named "example.txt" and prints the output to the current 640x480 window.

* The newline is handled internally and the current modelview matrix status isn't affected.
*/
class GL2DPainter
{
public:
   GL2DPainter();
   GL2DPainter(int w, int h);
   ~GL2DPainter();
   void init(int w, int h);
   void setColor(const GLfloat *pcolor);
   void setColor(int r, int g, int b, int a=255);
   void begin();
   void end();
   void drawPoint(int x, int y, double pointsize=1.0);
   void drawPoint(const vector<int> &x, const vector<int> &y, double pointsize=1.0);

   void drawLine(int x0, int y0, int x1, int y1, int linewidth=1);
   void drawPolygon(const vector<int> &pointsX, const vector<int> &pointsY, double linewidth=1.0, bool closedLoop=true);

private:
   bool mode2D;
   int width, height;
   GLfloat color[4];

};

#endif
