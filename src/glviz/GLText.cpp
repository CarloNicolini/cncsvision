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
#include <stdexcept>
#include "GLText.h"

using namespace std;

/** \ingroup GLVisualization
 * \brief Default constructor, you can declare a new GLText object without to need to have  a OpenGL rendering context enabled.
**/
GLText::GLText()
{  line=0;
    baseX=0;
    baseY=0;
    insideTextMode=false;
}

GLText::GLText(int w, int h,const GLfloat *pcolor, GLvoid *pfont )
{
    this->init(w,h,pcolor,pfont);
}

/** \ingroup GLVisualization
 * \brief Standard destructor
**/
GLText::~GLText()
{

}

void GLText::setXY(int x, int y)
{
    this->baseX=x;
    this->baseY=y;
    this->line=1;
}

/** \ingroup GLVisualization
 * \brief Initialize the font rendering context. You must give the current window size: width and height in pixel, specify the color of the text and optionally choose the font from one of the following fonts (the standard GLUT fonts)
 * <ul>
 * <li> GLUT_BITMAP_HELVETICA_10 </li>
 * <li> GLUT_BITMAP_HELVETICA_12 </li>
 * <li> GLUT_BITMAP_HELVETICA_18 </li>
 * <li> GLUT_BITMAP_TIMES_ROMAN_10 </li>
 * <li> GLUT_BITMAP_TIMES_ROMAN_24 </li>
 * </ul>
*/
void GLText::init(int w, int h,const GLfloat *pcolor, GLvoid *pfont )
{  width = w;
    height = h;
    color = (GLfloat*)pcolor;
    line=0;
    font= pfont;
    baseX=0;
    baseY=0;
}

/** \ingroup GLVisualization
*   \brief Enter the text input mode. You must call this method always before you draw text.
*
*   This method set the perspective matrix to the orthogonal perspective and allows to draw the text, this method is thought to speed up the program and avoid continous matrix change
**/
void GLText::enterTextInputMode()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);  //save the current color, blend function, alpha state
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_3D);
    glPushMatrix();
    //glClearColor(0.0,0.0,0.0,1.0);
    glMatrixMode (GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, this->width, 0, this->height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3fv(color);
    insideTextMode=true;
}

/** \ingroup GLVisualization
* \brief Leave the text input mode and restore the modelview matrix to previous state
**/
void GLText::leaveTextInputMode()
{  if ( insideTextMode )
    {
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glPopMatrix();
        glPopAttrib(); // restore previous color and lights state
    }
    else
    {		throw std::runtime_error( "Can't call this method if not just in textmode! Call GLText::enterTextInputMode before leaving it!");
    }
}


/** \ingroup GLVisualization
* \brief Draw the text to rendering window.
* First check if is inside the textmode, otherwise produce an error code
* \param text to draw with the color and font set previously via the init method.
**/
#ifdef __linux__
#include <GL/freeglut.h>
#endif
void GLText::draw(const string &text)
{  if ( insideTextMode )
    {  int x=20+baseX;
        int y=baseY+height-20*line-20;
        if ( font == GLUT_BITMAP_HELVETICA_12 )
            y=height-12*line-20;
        if ( font ==  GLUT_BITMAP_HELVETICA_18 )
            y=height-18*line-20;

        glRasterPos2f(x, y);
#ifdef WIN32
        int len = (int)text.length();
        for (int i = 0; i < len; i++)
        {  glutBitmapCharacter(font, text[i]);
        }
#endif
#ifdef __linux__
        // glutBitmapstring is much more efficient than glutBitmapCharacter
        glutBitmapString(font,(const unsigned char*)text.c_str());
#endif
        // Increment line
        line++;
    }
    else
        throw std::logic_error("You must be inside text mode: call GLText::enterTextInputMode() before!");
}

