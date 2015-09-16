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

// GLLight.cpp: implementation of the GLLight class.

#include <iostream>
#include <stdexcept>
#include "GLLight.h"

using namespace std;

GLint GLLight::lightIndex = 0;
const GLenum GLLight::lights[8] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};

void GLLight::enableLightining()
{  if ( glIsEnabled(GL_LIGHTING) == GL_FALSE )
   {  glEnable(GL_LIGHTING);
      glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
   }
}

void GLLight::disableLightining()
{  if ( glIsEnabled(GL_LIGHTING) == GL_TRUE )
      glDisable(GL_LIGHTING);
}

GLint GLLight::countLights()
{  return lightIndex;
}

GLLight::GLLight()
{  init();
}

GLLight::~GLLight()
{  --lightIndex;
}

void GLLight::init()
{  
   if ( lightIndex <= (GLint) (sizeof(lights) / sizeof(GLenum)) )
   {
      lightID =  lights[lightIndex++];
      for (int i = 0; i < 3; i++)
      {  ambient[i]  = 0.2f;
         diffuse[i]  = 0.2f;
         specular[i] = 0.2f;
      }
      ambient[3] = 1.0f;
      diffuse[3] = 1.0f;
      specular[3] = 0.2f;
      exponent=4.0f;
      cutoff=180.0f;
      attenuation1=1.0f;
      attenuation2=0.0f;
      attenuation3=0.0f;

      direction[0]=direction[1]=position[0]=position[1]=0.0f;
      direction[2]=position[2]=1.0f;

      lightReady=GL_FALSE;
      lightOn=GL_FALSE;
   }
   else
   {  throw std::runtime_error("A maximum of 8 light sources is allowed. Can't increment");
   }
}

void GLLight::on()
{  if ( lightOn == GL_FALSE )
   {  if ( lightReady == GL_FALSE )
      {  apply();
      }
      glEnable(lightID);
      lightOn = GL_TRUE;
   }
}

void GLLight::off()
{  if ( lightOn == GL_TRUE )
   {  glDisable(lightID);
      lightOn=GL_FALSE;
   }
}

bool GLLight::isOn()
{  return lightOn;
}

void GLLight::apply()
{  enableLightining();
   glLightfv(lightID, GL_POSITION,position );
   glLightfv(lightID, GL_DIFFUSE,  diffuse );
   glLightfv(lightID, GL_SPECULAR, specular );
   glLightfv(lightID, GL_AMBIENT,  ambient  );
   glLightfv(lightID, GL_SPOT_DIRECTION,  direction);
   glLightfv(lightID, GL_SPOT_CUTOFF,  &cutoff);
   glLightfv(lightID, GL_SPOT_EXPONENT,  &exponent);
   glLightfv(lightID, GL_CONSTANT_ATTENUATION,  &attenuation1);
   glLightfv(lightID, GL_LINEAR_ATTENUATION,    &attenuation2);
   glLightfv(lightID, GL_QUADRATIC_ATTENUATION, &attenuation3);
   lightReady=GL_TRUE;
}

void GLLight::preset(int lightType)
{

   switch (lightType)
   {  case LIGHT_PRESET_UNIFORM_WHITE :
         break;

      case LIGHT_PRESET_UNIFORM_RED :
         ambient[0]=1.0;
         ambient[1]=0.0;
         ambient[2]=0.0;
         ambient[3]=1.0;
         break;

      case LIGHT_PRESET_UNIFORM_BLUE :
         ambient[0]=0.0;
         ambient[1]=0.0;
         ambient[2]=1.0;
         ambient[3]=1.0;
         break;

      case LIGHT_PRESET_UNIFORM_GREEN :
         ambient[0]=0.0;
         ambient[1]=1.0;
         ambient[2]=0.0;
         ambient[3]=1.0;
         break;

      case LIGHT_PRESET_UNIFORM_GRAY:
         break;
   }
}


void GLLight::setDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat alpha )
{  diffuse[0]=r;
   diffuse[1]=g;
   diffuse[2]=b;
   diffuse[3]=alpha;
}

void GLLight::setDiffuse(GLfloat *v )
{  setDiffuse(v[0],v[1],v[2],v[3]);
}

void GLLight::setSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat alpha )
{  specular[0]=r;
   specular[1]=g;
   specular[2]=b;
   specular[3]=alpha;
}

void GLLight::setSpecular(GLfloat *v )
{  setSpecular(v[0],v[1],v[2],v[3]);
}

void GLLight::setAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat alpha )
{  ambient[0]=r;
   ambient[1]=g;
   ambient[2]=b;
   ambient[3]=alpha;
}

void GLLight::setPosition(GLfloat x, GLfloat y, GLfloat z)
{  position[0]=x;
   position[1]=y;
   position[2]=z;
}

void GLLight::setPosition(GLfloat *v )
{  setPosition(v[0],v[1],v[2]);
}

void GLLight::setDirection(GLfloat *v )
{  setDirection(v[0],v[1],v[2]);
}

void GLLight::setDirection(GLfloat x, GLfloat y, GLfloat z)
{  direction[0]=x;
   direction[1]=y;
   direction[2]=z;
}

void GLLight::setCutoff(GLfloat _cutoff )
{  cutoff = _cutoff;
}

