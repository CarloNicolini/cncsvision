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

// GLMaterial.h: interface for the GLMaterial class.

#ifndef _GLLIGHT_H_
#define _GLLIGHT_H_

#define LIGHT_PRESET_UNIFORM_WHITE 0
#define LIGHT_PRESET_UNIFORM_RED 1
#define LIGHT_PRESET_UNIFORM_BLUE 2
#define LIGHT_PRESET_UNIFORM_GREEN 3
#define LIGHT_PRESET_UNIFORM_GRAY 4

#include "IncludeGL.h"
/**
* \class GLLight
* \ingroup GLVisualization
* \brief GLLight handles the OpenGL lights with a variety of methods available. It's scoped and when the destructor is called,
* the light created is turned off, so there are no problems of lights superpositions.
**/
class GLLight
{
public:
   GLLight();
   ~GLLight();
   void init();
   void on();
   void off();
   bool isOn();
   void preset(int lightType);
   void apply();

   // get methods

   // set methods
   void setDiffuse(GLfloat,GLfloat,GLfloat,GLfloat);
   void setDiffuse(GLfloat* );
   void setSpecular(GLfloat,GLfloat,GLfloat,GLfloat);
   void setSpecular(GLfloat*);
   void setAmbient(GLfloat,GLfloat,GLfloat,GLfloat);
   void setAmbient(GLfloat*);

   void setPosition(GLfloat,GLfloat,GLfloat);
   void setPosition(GLfloat*);
   void setDirection(GLfloat,GLfloat,GLfloat);
   void setDirection(GLfloat*);

   void setExponent(GLfloat);
   void setCutoff(GLfloat);
   void setAttenuation(GLfloat,GLfloat,GLfloat);

   static GLint countLights();
   static void enableLightining();
   static void disableLightining();

   GLint getLightID()
   {  return lightID;
   };

protected:
   const static GLenum lights[8];
   static GLint lightIndex;

   GLenum lightType;
   GLint lightID;
   GLenum lightOn;
   GLenum lightReady;
   // diffuse color component
   GLfloat diffuse[4];
   // specular color component
   GLfloat specular[4];
   // ambient color component
   GLfloat ambient[4];
   // light position
   GLfloat position[3];
   GLfloat direction[3];
   // intensity distribution, 0 - uniform
   GLfloat exponent;
   // spread angle [0-90], 180
   GLfloat cutoff;

   // how intensity changes over distance
   GLfloat attenuation1;
   GLfloat attenuation2;
   GLfloat attenuation3;

};

#endif
