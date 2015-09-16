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

// GLMaterial.cpp: implementation of the GLMaterial class.

#include <iostream>
#include "GLMaterial.h"

using namespace std;

GLMaterial::GLMaterial()
{  for (int i = 0; i < 3; i++)
   {  ambient[i]  = 0;
      diffuse[i]  = 0;
      specular[i] = 0;
   }
   ambient[3] = 1.0f;
   diffuse[3] = 1.0f;
   specular[3] = 1.0f;
   shininess = 64;
}

GLMaterial::~GLMaterial()
{  reset();
   if ( glIsEnabled(GL_COLOR_MATERIAL) )
    glDisable(GL_COLOR_MATERIAL);
}

void GLMaterial::setDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat alpha )
{  diffuse[0]=r;
   diffuse[1]=g;
   diffuse[2]=b;
   diffuse[3]=alpha;
}

void GLMaterial::setDiffuse(GLfloat *v )
{  setDiffuse(v[0],v[1],v[2],v[3]);
}

void GLMaterial::setSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat alpha )
{  specular[0]=r;
   specular[1]=g;
   specular[2]=b;
   specular[3]=alpha;
}

void GLMaterial::setSpecular(GLfloat *v )
{  setSpecular(v[0],v[1],v[2],v[3]);
}
void GLMaterial::setAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat alpha )
{  ambient[0]=r;
   ambient[1]=g;
   ambient[2]=b;
   ambient[3]=alpha;
}

void GLMaterial::setAmbient(GLfloat *v )
{  setAmbient(v[0],v[1],v[2],v[3]);
}

void GLMaterial::reset()
{  preset(0);
}

void GLMaterial::apply()
{ if ( !glIsEnabled(GL_COLOR_MATERIAL) )
      glEnable(GL_COLOR_MATERIAL);

   glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
   glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void GLMaterial::preset(int materialType)
{
// Most material values from:
// http://www.ecs.umass.edu/ece/hill/ece661.dir/SDLbugFix.html
// Which cites:
// Source: McReynolds, Tom, and Blythe, David, "Programming with OpenGL: Advanced Rendering" . course Notes: SIGGRAPH ’97.

   switch (materialType)
   {  case MATERIAL_PRESET_MATTE_GRAY:
         ambient[0] = .5f;
         ambient[1] = .5f;
         ambient[2] = .5f;
         ambient[3] = 1.0f;
         diffuse[0] = .5f;
         diffuse[1] = .5f;
         diffuse[2] = .5f;
         diffuse[3] = 1.0f;
         specular[0] = 0.0f;
         specular[1] = 0.0f;
         specular[2] = 0.0f;
         specular[3] = 1.0f;
         shininess = 128.0f;
         break;
      case MATERIAL_PRESET_GOLD:
         ambient[0] = .24725f;
         ambient[1] = .1995f;
         ambient[2] = .0745f;
         ambient[3] = 1.0f;
         diffuse[0] = .75164f;
         diffuse[1] = .60648f;
         diffuse[2] = .22648f;
         diffuse[3] = 1.0f;
         specular[0] = .628281f;
         specular[1] = .555802f;
         specular[2] = .366065f;
         specular[3] = 1.0f;
         shininess = 51.2f;
         break;
      case MATERIAL_PRESET_POLISHED_GOLD:
         ambient[0] = .24725f;
         ambient[1] = .2245f;
         ambient[2] = .0645f;
         ambient[3] = 1.0f;
         diffuse[0] = .34615f;
         diffuse[1] = .3143f;
         diffuse[2] = .0903f;
         diffuse[3] = 1.0f;
         specular[0] = .797357f;
         specular[1] = .723991f;
         specular[2] = .208006f;
         specular[3] = 1.0f;
         shininess = 83.2f;
         break;
      case MATERIAL_PRESET_SILVER:
         ambient[0] = .19225f;
         ambient[1] = .19225f;
         ambient[2] = .19225f;
         ambient[3] = 1.0f;
         diffuse[0] = .50754f;
         diffuse[1] = .50754f;
         diffuse[2] = .50754f;
         diffuse[3] = 1.0f;
         specular[0] = .508273f;
         specular[1] = .508273f;
         specular[2] = .508273f;
         specular[3] = 1.0f;
         shininess = 51.2f;
         break;
      case MATERIAL_PRESET_POLISHED_SILVER:
         ambient[0] = .23125f;
         ambient[1] = .23125f;
         ambient[2] = .23125f;
         ambient[3] = 1.0f;
         diffuse[0] = .2775f;
         diffuse[1] = .2775f;
         diffuse[2] = .2775f;
         diffuse[3] = 1.0f;
         specular[0] = .773911f;
         specular[1] = .773911f;
         specular[2] = .773911f;
         specular[3] = 1.0f;
         shininess = 89.6f;
         break;
      case MATERIAL_PRESET_BRONZE:
         ambient[0] = .2125f;
         ambient[1] = .1275f;
         ambient[2] = .054f;
         ambient[3] = 1.0f;
         diffuse[0] = .714f;
         diffuse[1] = .4284f;
         diffuse[2] = .18144f;
         diffuse[3] = 1.0f;
         specular[0] = .393548f;
         specular[1] = .271906f;
         specular[2] = .166721f;
         specular[3] = 1.0f;
         shininess = 25.6f;
         break;
      case MATERIAL_PRESET_POLISHED_BRONZE:
         ambient[0] = .25f;
         ambient[1] = .148f;
         ambient[2] = .06475f;
         ambient[3] = 1.0f;
         diffuse[0] = .4f;
         diffuse[1] = .2368f;
         diffuse[2] = .1036f;
         diffuse[3] = 1.0f;
         specular[0] = .774597f;
         specular[1] = .458561f;
         specular[2] = .200621f;
         specular[3] = 1.0f;
         shininess = 76.8f;
         break;
      case MATERIAL_PRESET_BRASS:
         ambient[0] = .329412f;
         ambient[1] = .223529f;
         ambient[2] = .027451f;
         diffuse[0] = .780392f;
         diffuse[1] = .568627f;
         diffuse[2] = .113725f;
         diffuse[3] = 1.0f;
         specular[0] = .992157f;
         specular[1] = .941176f;
         specular[2] = .807843f;
         specular[3] = 1.0f;
         shininess = 27.8974f;
         break;
      case MATERIAL_PRESET_COPPER:
         ambient[0] = .19125f;
         ambient[1] = .0735f;
         ambient[2] = .0225f;
         ambient[3] = 1.0f;
         diffuse[0] = .7038f;
         diffuse[1] = .27048f;
         diffuse[2] = .0828f;
         diffuse[3] = 1.0f;
         specular[0] = .256777f;
         specular[1] = .137622f;
         specular[2] = .086014f;
         specular[3] = 1.0f;
         shininess = 12.8f;
         break;
      case MATERIAL_PRESET_POLISHED_COPPER:
         ambient[0] = .2295f;
         ambient[1] = .08825f;
         ambient[2] = .0275f;
         ambient[3] = 1.0f;
         diffuse[0] = .5508f;
         diffuse[1] = .2118f;
         diffuse[2] = .066f;
         diffuse[3] = 1.0f;
         specular[0] = .580594f;
         specular[1] = .223257f;
         specular[2] = .0695701f;
         specular[3] = 1.0f;
         shininess = 51.2f;
         break;
      case MATERIAL_PRESET_PEWTER:
         ambient[0] = .10588f;
         ambient[1] = .058824f;
         ambient[2] = .113725f;
         ambient[3] = 1.0f;
         diffuse[0] = .427451f;
         diffuse[1] = .470588f;
         diffuse[2] = .541176f;
         diffuse[3] = 1.0f;
         specular[0] = .3333f;
         specular[1] = .3333f;
         specular[2] = .521569f;
         specular[3] = 1.0f;
         shininess = 9.84615f;
         break;
      case MATERIAL_PRESET_CHROME:
         ambient[0] = .25f;
         ambient[1] = .25f;
         ambient[2] = .25f;
         ambient[3] = 1.0f;
         diffuse[0] = .4f;
         diffuse[1] = .4f;
         diffuse[2] = .4f;
         diffuse[3] = 1.0f;
         specular[0] = .774597f;
         specular[1] = .774597f;
         specular[2] = .774597f;
         specular[3] = 1.0f;
         shininess = 76.8f;
         break;
      case MATERIAL_PRESET_EMERALD:
         ambient[0] = .0215f;
         ambient[1] = .1745f;
         ambient[2] = .0215f;
         ambient[3] = 0.55f;
         diffuse[0] = .07568f;
         diffuse[1] = .61424f;
         diffuse[2] = .07568f;
         diffuse[3] = 0.55f;
         specular[0] = .633f;
         specular[1] = .727811f;
         specular[2] = .633f;
         specular[3] = 0.55f;
         shininess = 76.8f;
         break;
      case MATERIAL_PRESET_JADE:
         ambient[0] = .135f;
         ambient[1] = .2225f;
         ambient[2] = .1575f;
         ambient[3] = 0.95f;
         diffuse[0] = .54f;
         diffuse[1] = .89f;
         diffuse[2] = .63f;
         diffuse[3] = 0.95f;
         specular[0] = .316228f;
         specular[1] = .316228f;
         specular[2] = .316228f;
         specular[3] = 0.95f;
         shininess = 12.8f;
         break;
      case MATERIAL_PRESET_OBSIDIAN:
         ambient[0] = .05375f;
         ambient[1] = .05f;
         ambient[2] = .06625f;
         ambient[3] = 0.82f;
         diffuse[0] = .18275f;
         diffuse[1] = .17f;
         diffuse[2] = .22525f;
         diffuse[3] = 0.82f;
         specular[0] = .332741f;
         specular[1] = .328634f;
         specular[2] = .346435f;
         specular[3] = 0.82f;
         shininess = 38.4f;
         break;
      case MATERIAL_PRESET_PEARL:
         ambient[0] = .25f;
         ambient[1] = .20725f;
         ambient[2] = .20725f;
         ambient[3] = 0.922f;
         diffuse[0] = 1.0f;
         diffuse[1] = .829f;
         diffuse[2] = .829f;
         diffuse[3] = 0.922f;
         specular[0] = .296648f;
         specular[1] = .296648f;
         specular[2] = .296648f;
         specular[3] = 0.922f;
         shininess = 11.264f;
         break;
      case MATERIAL_PRESET_RUBY:
         ambient[0] = .1745f;
         ambient[1] = .01175f;
         ambient[2] = .01175f;
         ambient[3] = 0.55f;
         diffuse[0] = .61424f;
         diffuse[1] = .04136f;
         diffuse[2] = .04136f;
         diffuse[3] = 0.55f;
         specular[0] = .727811f;
         specular[1] = .626959f;
         specular[2] = .626959f;
         specular[3] = 0.55f;
         shininess = 76.8f;
         break;
      case MATERIAL_PRESET_TURQUOISE:

         ambient[0] = .1f;
         ambient[1] = .18725f;
         ambient[2] = .1745f;
         ambient[3] = 0.8f;
         diffuse[0] = .396f;
         diffuse[1] = .74151f;
         diffuse[2] = .69102f;
         diffuse[3] = 0.8f;
         specular[0] = .297254f;
         specular[1] = .30829f;
         specular[2] = .306678f;
         specular[3] = 0.8f;
         shininess = 12.8f;
         break;
      case MATERIAL_PRESET_BLACK_PLASTIC:
         ambient[0] = 0.0f;
         ambient[1] = 0.0f;
         ambient[2] = 0.0f;
         ambient[3] = 1.0f;
         diffuse[0] = .01f;
         diffuse[1] = .01f;
         diffuse[2] = .01f;
         diffuse[3] = 1.0f;
         specular[0] = .5f;
         specular[1] = .5f;
         specular[2] = .5f;
         specular[3] = 1.0f;
         shininess = 32.0f;
         break;
      case MATERIAL_PRESET_BLACK_RUBBER:
         ambient[0] = 0.02f;
         ambient[1] = 0.02f;
         ambient[2] = 0.02f;
         ambient[3] = 1.0f;
         diffuse[0] = .01f;
         diffuse[1] = .01f;
         diffuse[2] = .01f;
         diffuse[3] = 1.0f;
         specular[0] = .4f;
         specular[1] = .4f;
         specular[2] = .4f;
         specular[3] = 1.0f;
         shininess = 10.0f;
         break;
      case MATERIAL_PRESET_DEFAULT_OUTSIDE:
         ambient[0] = 0.0f;
         ambient[1] = 0.0f;
         ambient[2] = 0.7f;
         ambient[3] = 1.0f;
         diffuse[0] = 0.0f;
         diffuse[1] = 0.0f;
         diffuse[2] = 0.7f;
         diffuse[3] = 1.0f;
         specular[0] = 0.7f;
         specular[1] = 0.7f;
         specular[2] = 0.7f;
         specular[3] = 1.0f;
         shininess = 50.0f;
         break;
      case MATERIAL_PRESET_DEFAULT_INSIDE:
         ambient[0] = .7f;
         ambient[1] = 0.0f;
         ambient[2] = 0.0f;
         ambient[3] = 1.0f;
         diffuse[0] = .7f;
         diffuse[1] = 0.0f;
         diffuse[2] = 0.0f;
         diffuse[3] = 1.0f;
         specular[0] = 0.7f;
         specular[1] = 0.7f;
         specular[2] = 0.7f;
         specular[3] = 1.0f;
         shininess = 50.0f;
         break;
      default:
         ambient[0] = .5f;
         ambient[1] = .5f;
         ambient[2] = .5f;
         ambient[3] = 1.0f;
         diffuse[0] = .5f;
         diffuse[1] = .5f;
         diffuse[2] = .5f;
         diffuse[3] = 1.0f;
         specular[0] = 0.0f;
         specular[1] = 0.0f;
         specular[2] = 0.0f;
         specular[3] = 1.0f;
         shininess = 128.0f;
         break;
   }

}
