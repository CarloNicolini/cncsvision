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
//
// OpenGL Texture Class
// by: Matthew Fairfax
//
// GLTexture.h: interface for the GLTexture class.
// This class loads a texture file and prepares it
// to be used in OpenGL. It can open a bitmap or a
// targa file. The min filter is set to mipmap b/c
// they look better and the performance cost on
// modern video cards in negligible. I leave all of
// the texture management to the application. I have
// included the ability to load the texture from a
// Visual Studio resource. The bitmap's id must be
// be surrounded by quotation marks (i.e. "Texture.bmp").
// The targa files must be in a resource type of "TGA"
// (including the quotes). The targa's id must be
// surrounded by quotation marks (i.e. "Texture.tga").
//
// Usage:
// GLTexture tex;
// GLTexture tex1;
// GLTexture tex3;
//
// tex.Load("texture.bmp"); // Loads a bitmap
// tex.Use();				// Binds the bitmap for use
// 
// tex1.LoadFromResource("texture.tga"); // Loads a targa
// tex1.Use();				 // Binds the targa for use
//
// // You can also build a texture with a single color and use it
// tex3.BuildColorTexture(255, 0, 0);	// Builds a solid red texture
// tex3.Use();				 // Binds the targa for use
//
//////////////////////////////////////////////////////////////////////

#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream> 
#include "IncludeGL.h"
using std::cerr;
using std::endl;
using std::cout;
using std::string;

/**
 * @brief The GLTexture class
 */
class GLTexture  
{
public:
    GLTexture();
    ~GLTexture();
    void loadFromResource(const char *name);				// Load the texture from a resource
    void loadBMP(const char *name);						// Loads a bitmap file
    void load(const char *name);							// Load the texture
    void use();										// Binds the texture for use
    void use(unsigned int textureID);
    void buildColorTexture(unsigned char r, unsigned char g, unsigned char b);	// Sometimes we want a texture of uniform color
    void drawFrame(double width, double height, double distance);
    void discard();
private:
    char *texturename;								// The textures name
    unsigned int texture[1];						// OpenGL's number for the texture
    int width;										// Texture's width
    int height;										// Texture's height
    // Actual RGB data
    unsigned char * data;
};

#endif // GLTEXTURE_H
