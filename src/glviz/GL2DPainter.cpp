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

#include <cassert>
#include <iostream>
#include "GLUtils.h"
#include "GL2DPainter.h"

using namespace std;

/** \ingroup GLVisualization
 * \brief Default constructor, you can declare a new GL2DPainter object without to need to have  a OpenGL rendering context enabled.
**/
GL2DPainter::GL2DPainter()
{  mode2D=false;
   color[0]=color[1]=color[2]=color[3]=1.0;
   this->init(1024,768);
}

GL2DPainter::GL2DPainter(int w, int h)
{  mode2D=false;
   color[0]=color[1]=color[2]=color[3]=1.0;
   this->init(w,h);
}

/** \ingroup GLVisualization
 * \brief Standard destructor
**/
GL2DPainter::~GL2DPainter()
{  end();
}

/** \ingroup GLVisualization
 * \brief Init the painter 2D area
*/
void GL2DPainter::init(int w, int h )
{  width = w;
   height = h;
}


void GL2DPainter::setColor(const GLfloat *pcolor)
{  color[0]=pcolor[0];
   color[1]=pcolor[1];
   color[2]=pcolor[2];
   color[3]=pcolor[3];
}

void GL2DPainter::setColor(int r, int g, int b, int alpha)
{  color[0] = (double)r/255.0;
   color[1] = (double)g/255.0;
   color[2] = (double)b/255.0;
   color[3]= (double)alpha/255.0;
}

/** \ingroup GLVisualization
*   \brief Enter the text input mode. You must call this method always before you draw text.
*
*   This method set the perspective matrix to the orthogonal perspective and allows to draw the text, this method is thought to speed up the program and avoid continous matrix change
**/
void GL2DPainter::begin()
{  if ( ! mode2D )
   {  glPushAttrib(GL_COLOR_BUFFER_BIT);  //save the current color, blend function, alpha state
      glPushAttrib(GL_LIGHTING_BIT);   // save the current light state

      glDisable(GL_BLEND);
      glDisable(GL_LIGHTING);
      glPushMatrix();
      glClearColor(0.0,0.0,0.0,1.0);
      glMatrixMode (GL_PROJECTION);
      glPushMatrix ();
      glLoadIdentity ();
      glOrtho (0, width, height, 0, 0, 1);
      glMatrixMode (GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
   }
   mode2D=true;
}

/** \ingroup GLVisualization
* \brief Leave the text input mode and restore the modelview matrix to previous state
**/
void GL2DPainter::end()
{  if ( mode2D )
   {  glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glPopMatrix();

      glPopAttrib(); // restore previous lights state
      glPopAttrib(); //restore previous color state
   }
   else
      cerr << "Can't call this method if not just in textmode! Call GL2DPainter::enterTextInputMode before leaving it!" << endl;
}

void GL2DPainter::drawPoint(int x, int y, double pointsize)
{  glPointSize(pointsize);
   glColor4fv(color);
   glBegin(GL_POINTS);
   glVertex2i(x,y);
   glEnd();
}

void GL2DPainter::drawPoint(const vector<int> &x, const vector<int> &y, double pointsize)
{  unsigned int n=x.size();
   assert(x.size()==y.size());
   glPointSize(pointsize);
   glColor4fv(color);
   glBegin(GL_POINTS);
   for (unsigned int i=0; i<n; i++)
      glVertex2i(x[i],y[i]);

   glEnd();
}

void GL2DPainter::drawLine(int x0, int y0, int x1, int y1, int linewidth)
{  glLineWidth(linewidth);
   glColor4fv(color);
   glBegin(GL_LINES);
   glVertex2i(x0,y0);
   glVertex2i(x1,y1);
   glEnd();
}

void GL2DPainter::drawPolygon(const vector<int> &x, const vector<int> &y, double linewidth, bool closedLoop)
{  unsigned int n=x.size();

   assert(x.size()==y.size());

   glLineWidth(linewidth);
   glColor4fv(color);

   if (closedLoop )
      glBegin(GL_LINE_LOOP);
   else
      glBegin(GL_LINE_STRIP);
   for (unsigned int i=0; i<n; i++)
      glVertex2i(x[i],y[i]);

   glEnd();
}


