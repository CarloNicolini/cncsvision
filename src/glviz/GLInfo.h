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

///////////////////////////////////////////////////////////////////////////////
// GLInfo.h
// ========
// get GL vendor, version, supported extensions and other states using glGet*
// functions and store them GLInfo struct variable
//
// To get valid OpenGL infos, OpenGL rendering context (RC) must be opened
// before calling GLInfo::getInfo(). Otherwise it returns false.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2005-10-04
// UPDATED: 2009-10-07
//
// Copyright (c) 2005 Song Ho Ahn
///////////////////////////////////////////////////////////////////////////////

#ifndef GLINFO_H
#define GLINFO_H
#include <string>
#include <vector>
#include "IncludeGL.h"
// struct variable to store OpenGL info
struct GLInfo
{  std::string vendor;
   std::string renderer;
   std::string version;
   std::vector <std::string> extensions;
   int redBits;
   int greenBits;
   int blueBits;
   int alphaBits;
   int depthBits;
   int stencilBits;
   int maxTextureSize;
   int maxLights;
   int maxAttribStacks;
   int maxModelViewStacks;
   int maxProjectionStacks;
   int maxClipPlanes;
   int maxTextureStacks;

   // ctor, init all members
   GLInfo() : redBits(0), greenBits(0), blueBits(0), alphaBits(0), depthBits(0),
      stencilBits(0), maxTextureSize(0), maxLights(0), maxAttribStacks(0),
      maxModelViewStacks(0), maxClipPlanes(0), maxTextureStacks(0) {}

   bool getInfo();                             // extract info
   void printSelf();                           // print itself
   bool isExtensionSupported(const std::string& ext); // check if a extension is supported
};

#endif
