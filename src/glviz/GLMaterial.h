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

#ifndef _GLMATERIAL_H_
#define _GLMATERIAL_H_
#include "IncludeGL.h"

#define MATERIAL_PRESET_DEFAULT_OUTSIDE           0
#define MATERIAL_PRESET_DEFAULT_INSIDE         1
#define MATERIAL_PRESET_MATTE_GRAY     2
#define MATERIAL_PRESET_GOLD        3
#define MATERIAL_PRESET_POLISHED_GOLD          4
#define MATERIAL_PRESET_SILVER         5
#define MATERIAL_PRESET_POLISHED_SILVER           6
#define MATERIAL_PRESET_BRONZE         7
#define MATERIAL_PRESET_POLISHED_BRONZE         8
#define MATERIAL_PRESET_COPPER         9
#define MATERIAL_PRESET_POLISHED_COPPER           10
#define MATERIAL_PRESET_BRASS       11
#define MATERIAL_PRESET_CHROME         12
#define MATERIAL_PRESET_PEWTER         13
#define MATERIAL_PRESET_EMERALD        14
#define MATERIAL_PRESET_JADE        15
#define MATERIAL_PRESET_OBSIDIAN    16
#define MATERIAL_PRESET_PEARL       17
#define MATERIAL_PRESET_RUBY        18
#define MATERIAL_PRESET_TURQUOISE      19
#define MATERIAL_PRESET_BLACK_PLASTIC          20
#define MATERIAL_PRESET_BLACK_RUBBER           21
#define MATERIAL_NUM_PRESETS        22

/**
* \class GLMaterial
* \ingroup GLVisualization
* \brief GLMaterial handles the material via some simple material specifications.
* It's scoped and when the destructor is called it restore the material to the previous one.
**/
class GLMaterial
{
public:
   GLMaterial();
   ~GLMaterial();
   void reset();
   void apply();
   void preset(int materialType);
   // set methods
   void setDiffuse(GLfloat,GLfloat,GLfloat,GLfloat);
   void setDiffuse(GLfloat* );
   void setSpecular(GLfloat,GLfloat,GLfloat,GLfloat);
   void setSpecular(GLfloat*);
   void setAmbient(GLfloat,GLfloat,GLfloat,GLfloat);
   void setAmbient(GLfloat*);
   void setShininess(GLfloat val)
   {
       shininess= val;
   };

   GLfloat ambient[4];
   GLfloat diffuse[4];
   GLfloat specular[4];
   GLfloat shininess;

   static const int MaterialDefaultOutside = MATERIAL_PRESET_DEFAULT_OUTSIDE;
   static const int MaterialDefaultInside = MATERIAL_PRESET_DEFAULT_INSIDE;
   static const int MaterialMatteGray = MATERIAL_PRESET_MATTE_GRAY;
   static const int MaterialGold = MATERIAL_PRESET_GOLD;
   static const int MaterialPolishedGold = MATERIAL_PRESET_POLISHED_GOLD;
   static const int MaterialSilver = MATERIAL_PRESET_SILVER;
   static const int MaterialPolishedSilver = MATERIAL_PRESET_POLISHED_SILVER;
   static const int MaterialBronze = MATERIAL_PRESET_BRONZE;
   static const int MaterialPolishedBronze = MATERIAL_PRESET_POLISHED_BRONZE;
   static const int MaterialCopper = MATERIAL_PRESET_COPPER;
   static const int MaterialPolishedCopper = MATERIAL_PRESET_POLISHED_COPPER;
   static const int MaterialBrass = MATERIAL_PRESET_BRASS;
   static const int MaterialChrome = MATERIAL_PRESET_CHROME;
   static const int MaterialPewter = MATERIAL_PRESET_PEWTER;
   static const int Emerald = MATERIAL_PRESET_EMERALD;
   static const int MaterialJade = MATERIAL_PRESET_JADE;
   static const int MaterialObsidian = MATERIAL_PRESET_OBSIDIAN;
   static const int MaterialPearl = MATERIAL_PRESET_PEARL;
   static const int MaterialTurquoise = MATERIAL_PRESET_TURQUOISE;
   static const int MaterialBlackPlastic = MATERIAL_PRESET_BLACK_PLASTIC;
   static const int MaterialBlackRubber = MATERIAL_PRESET_BLACK_RUBBER;
   static const int MaterialPresets = MATERIAL_NUM_PRESETS;
};


#endif
