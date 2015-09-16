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
// GLInfo.cpp
// ==========
// get GL vendor, version, supported extensions and other states using glGet*
// functions and store them GLInfo struct variable
//
// To get valid OpenGL infos, OpenGL rendering context (RC) must be opened
// before calling GLInfo::getInfo(). Otherwise it returns false.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2005-10-04
// UPDATED: 2009-10-06
//
// Copyright (c) 2005 Song Ho Ahn
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include "GLInfo.h"


///////////////////////////////////////////////////////////////////////////////
// extract openGL info
// This function must be called after GL rendering context opened.
///////////////////////////////////////////////////////////////////////////////
bool GLInfo::getInfo()
{  char* str = 0;
   char* tok = 0;

   // get vendor string
   str = (char*)glGetString(GL_VENDOR);
   if(str) this->vendor = str;                  // check NULL return value
   else return false;

   // get renderer string
   str = (char*)glGetString(GL_RENDERER);
   if(str) this->renderer = str;                // check NULL return value
   else return false;

   // get version string
   str = (char*)glGetString(GL_VERSION);
   if(str) this->version = str;                 // check NULL return value
   else return false;

   // get all extensions as a string
   str = (char*)glGetString(GL_EXTENSIONS);

   // split extensions
   if(str)
   {  tok = strtok((char*)str, " ");
      while(tok)
      {  this->extensions.push_back(tok);    // put a extension into struct
         tok = strtok(0, " ");               // next token
      }
   }
   else
   {  return false;
   }

   // sort extension by alphabetical order
   std::sort(this->extensions.begin(), this->extensions.end());

   // get number of color bits
   glGetIntegerv(GL_RED_BITS, &this->redBits);
   glGetIntegerv(GL_GREEN_BITS, &this->greenBits);
   glGetIntegerv(GL_BLUE_BITS, &this->blueBits);
   glGetIntegerv(GL_ALPHA_BITS, &this->alphaBits);

   // get depth bits
   glGetIntegerv(GL_DEPTH_BITS, &this->depthBits);

   // get stecil bits
   glGetIntegerv(GL_STENCIL_BITS, &this->stencilBits);

   // get max number of lights allowed
   glGetIntegerv(GL_MAX_LIGHTS, &this->maxLights);

   // get max texture resolution
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &this->maxTextureSize);

   // get max number of clipping planes
   glGetIntegerv(GL_MAX_CLIP_PLANES, &this->maxClipPlanes);

   // get max modelview and projection matrix stacks
   glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &this->maxModelViewStacks);
   glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &this->maxProjectionStacks);
   glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &this->maxAttribStacks);

   // get max texture stacks
   glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &this->maxTextureStacks);

   return true;
}



///////////////////////////////////////////////////////////////////////////////
// check if the video card support a certain extension
///////////////////////////////////////////////////////////////////////////////
bool GLInfo::isExtensionSupported(const std::string& ext)
{  // search corresponding extension
   std::vector<std::string>::const_iterator iter = this->extensions.begin();
   std::vector<std::string>::const_iterator endIter = this->extensions.end();

   while(iter != endIter)
   {  if(ext == *iter)
         return true;
      else
         ++iter;
   }
   return false;
}



///////////////////////////////////////////////////////////////////////////////
// print OpenGL info to screen and save to a file
///////////////////////////////////////////////////////////////////////////////
void GLInfo::printSelf()
{  std::stringstream ss;

   ss << std::endl; // blank line
   ss << "OpenGL Driver Info" << std::endl;
   ss << "==================" << std::endl;
   ss << "Vendor: " << this->vendor << std::endl;
   ss << "Version: " << this->version << std::endl;
   ss << "Renderer: " << this->renderer << std::endl;

   ss << std::endl;
   ss << "Color Bits(R,G,B,A): (" << this->redBits << ", " << this->greenBits
      << ", " << this->blueBits << ", " << this->alphaBits << ")\n";
   ss << "Depth Bits: " << this->depthBits << std::endl;
   ss << "Stencil Bits: " << this->stencilBits << std::endl;

   ss << std::endl;
   ss << "Max Texture Size: " << this->maxTextureSize << "x" << this->maxTextureSize << std::endl;
   ss << "Max Lights: " << this->maxLights << std::endl;
   ss << "Max Clip Planes: " << this->maxClipPlanes << std::endl;
   ss << "Max Modelview Matrix Stacks: " << this->maxModelViewStacks << std::endl;
   ss << "Max Projection Matrix Stacks: " << this->maxProjectionStacks << std::endl;
   ss << "Max Attribute Stacks: " << this->maxAttribStacks << std::endl;
   ss << "Max Texture Stacks: " << this->maxTextureStacks << std::endl;

   ss << std::endl;
   ss << "Total Number of Extensions: " << this->extensions.size() << std::endl;
   ss << "==============================" << std::endl;
   for(unsigned int i = 0; i < this->extensions.size(); ++i)
      ss << this->extensions.at(i) << std::endl;

   ss << "======================================================================" << std::endl;

   std::cout << ss.str() << std::endl;
}
