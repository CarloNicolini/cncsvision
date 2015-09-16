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

//////////////////////////////////////////////////////////////////////
//
// OpenGL Texture Class
// by: Matthew Fairfax
//
// GLTexture.cpp: implementation of the GLTexture class.
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
// tex1.loadFromResource("texture.tga"); // Loads a targa
// tex1.use();				 // Binds the targa for use
//
// // You can also build a texture with a single color and use it
// tex3.buildColorTexture(255, 0, 0);	// Builds a solid red texture
// tex3.use();				 // Binds the targa for use


#include <string>
#include <stdexcept>
#include "GLTexture.h"

/**
 * @brief GLTexture::GLTexture
 */
GLTexture::GLTexture()
{
    this->texture[0]=0;
    this->data=NULL;
}

/**
 * @brief GLTexture::~GLTexture
 */
GLTexture::~GLTexture()
{
    if (texture[0]!=0)
    {
        this->discard();
        glDeleteTextures(1,&texture[0]);
    }
    if (this->data)
        delete[] this->data;
}

/**
 * @brief GLTexture::load
 * @param name
 */
void GLTexture::load(const char *name)
{
    // make the texture name all lower case
    //texturename = strlwr(strdup(name));
    texturename = strdup(name);
    // strip "'s
    if (strstr(texturename, "\""))
        texturename = strtok(texturename, "\"");

    // check the file extension to see what type of texture
    if(strstr(texturename, ".bmp"))
        loadBMP(texturename);
}

/**
 * @brief GLTexture::loadFromResource
 * @param name
 */
void GLTexture::loadFromResource(const char *name)
{
    // make the texture name all lower case
    //texturename = strlwr(strdup(name));
    texturename = strdup(name);
    // check the file extension to see what type of texture
    if(strstr(texturename, ".bmp"))
        loadBMP(name);

}

/**
 * @brief GLTexture::drawFrame
 * @param width
 * @param height
 * @param distance
 */
void GLTexture::drawFrame(double width, double height, double distance)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glColor3f(1,1,1);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    this->use();
    glBegin(GL_QUADS);
    glTexCoord2d(0,0);
    glVertex3d(-width/2,-height/2,distance);

    glTexCoord2d(1,0);
    glVertex3d(width/2,-height/2,distance);

    glTexCoord2d(1,1);
    glVertex3d(width/2,height/2,distance);

    glTexCoord2d(0,1);
    glVertex3d(-width/2,height/2,distance);
    glEnd();
    this->discard();
    glPopAttrib();
}

/**
 * @brief GLTexture::use
 */
void GLTexture::use()
{
    glEnable(GL_TEXTURE_2D);								// Enable texture mapping
    glBindTexture(GL_TEXTURE_2D, texture[0]);				// Bind the texture as the current one
}

/**
 * @brief GLTexture::use
 * @param textureID
 */
void GLTexture::use(unsigned int textureID)
{
    glEnable(GL_TEXTURE_2D);								// Enable texture mapping
    glBindTexture(GL_TEXTURE_2D, textureID);				// Bind the texture as the current one
}

/**
 * @brief GLTexture::discard
 */
void GLTexture::discard()
{
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);
}

/**
 * @brief GLTexture::loadBMP
 * @param filename
 */
void GLTexture::loadBMP(const char *filename)
{
    if (texture[0]!=0)
    {
        this->discard();
        glDeleteTextures(1,&texture[0]);
    }
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    //unsigned int width, height;
    unsigned int imageSize;   // = width*height*3

    // Open the file
    FILE * file = fopen(filename,"rb");
    if (!file)
    {
        cerr << "ERROR == File " << string(filename) << " not found ===" << endl;
        throw std::runtime_error("File "+string(filename) + " not found");
        return ;
    }

    if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
        cerr << "Not a correct BMP file" << endl;
        fclose(file);
        return ;
    }

    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)
        imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)
        dataPos=54; // The BMP header is done that way

    // Create a buffer
    if (data==NULL)
    {
        data = new unsigned char [imageSize];
    }
    else
    {
        delete[] data;
        data = new unsigned char [imageSize];
    }
    // Read the actual data from the file into the buffer
    size_t x = fread(data,1,imageSize,file);

    //Everything is in memory now, the file can be closed
    fclose(file);

    //create texture
    glGenTextures( 1, &texture[0] ); //generate the texture with the loaded data
    glBindTexture( GL_TEXTURE_2D, texture[0] ); //bind the texture to it’s array
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameter

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );

    //Here we are setting the parameter to repeat the texture instead of clamping the texture
    //to the edge of our shape.
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    //Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
    /* Use mipmapping filter
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // Generate the mipmaps
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
  **/
}

/**
 * @brief GLTexture::buildColorTexture
 * @param r
 * @param g
 * @param b
 */
void GLTexture::buildColorTexture(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char data[12];	// a 2x2 texture at 24 bits
    // Store the data
    for(int i = 0; i < 12; i += 3)
    {
        data[i] = r;
        data[i+1] = g;
        data[i+2] = b;
    }
    // Generate the OpenGL texture id
    glGenTextures(1, &texture[0]);
    // Bind this texture to its id
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Use mipmapping filter
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    // Generate the texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 2, 2, GL_RGB, GL_UNSIGNED_BYTE, data);
}

