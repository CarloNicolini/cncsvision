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

/* ----------------------------------------------------------------------------
Copyright (c) 2007, Wojciech Jozefowicz, Toby Howard <tobyhoward@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * The name of the author may not be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

#include <string>
#include <cstdio>

#include "FramerateDisplayer.h"

FramerateDisplayer::FramerateDisplayer()
{  d_text[0]='\0';
   sprintf(d_text,"Framerate: ");
   d_pointer_p = d_text+11;
   //d_timer.Initialize();
   d_timer.start();
}

void FramerateDisplayer::init(float timeStep, void* fonts,float red, float green, float blue, float x, float y)
{  d_frames = 0;
   d_timeStep = timeStep;
   d_fonts_p = fonts;
   d_red = red;
   d_green = green;
   d_blue = blue;
   d_x = x;
   d_y = y;
   //d_timer.SetReference();
   d_timer.start();
}

void FramerateDisplayer::displayFramerate()
{  GLint matrixMode;
   GLboolean lightingOn;

   lightingOn= glIsEnabled(GL_LIGHTING);
   if (lightingOn) glDisable(GL_LIGHTING);

   glGetIntegerv(GL_MATRIX_MODE, &matrixMode);

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0.0, 1.0, 0.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glPushAttrib(GL_COLOR_BUFFER_BIT);
   glColor3f(d_red, d_green, d_blue);
   glRasterPos3f(d_x, d_y, 0.0);
   for(char *ch = d_text; *ch; ch++)
   {  glutBitmapCharacter(d_fonts_p, (int)*ch);
   }
   glPopAttrib();
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();

   glMatrixMode(matrixMode);
   if (lightingOn) glEnable(GL_LIGHTING);
}

void FramerateDisplayer::anotherFrameExecuted()
{  d_frames++;
   float temp_time = d_timer.getElapsedTimeInMilliSec();
   if( temp_time > d_timeStep )
   {  sprintf(d_pointer_p,"%d",(int)(d_frames/temp_time));
      d_frames = 0;
      d_timer.start();
   }
}

void FramerateDisplayer::reset()
{  d_frames = 0;
   d_timer.start();
}
