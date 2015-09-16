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


#ifndef _GLTEXT_H_
#define _GLTEXT_H_
#include <iostream>
#include <string>
#include "IncludeGL.h"

using namespace std;

/**
 * \ingroup GLVisualization
 * \class GLText
 * \brief GLText is a wrapper around the GLUT calls to font rendering, made simple for use.
 *
 * GLText allows you to specify only the screen size and the string you want to render and the text is rendered of the color you want and quite fast.
 * An example is the following function that reads from an input file named "example.txt" and prints the output to the current 640x480 window.
* \code
* void drawText()
* {
*    ifstream file;
*    file.open("example.txt");
*    string line;
*
*    GLText text;
*    GLfloat glRed[]  = { 1.0, 0.0, 0.0, 1.0 };
*    text.init(640,480,glRed, GLUT_BITMAP_HELVETICA_12);
*    text.enterTextInputMode();
*    while ( !file.eof() )
*    {
*    getline(file,line);
*    text.draw(line);
*    }
*    text.leaveTextInputMode();
* }
* \endcode
* The newline is handled internally and the current modelview matrix status isn't affected.
*/
class GLText
{
public:
   GLText();
   GLText(int w, int h, const GLfloat *color, GLvoid* font=GLUT_BITMAP_HELVETICA_18);
   ~GLText();
   void init(int w, int h, const GLfloat *color, GLvoid* font=GLUT_BITMAP_HELVETICA_18);
   void setXY(int x, int y);
   void enterTextInputMode();
   void leaveTextInputMode();
   void draw(const string &);

private:
   bool insideTextMode;
   int baseX,baseY;
   int width, height,line;
   GLfloat *color;
   GLvoid *font;

};

#endif
