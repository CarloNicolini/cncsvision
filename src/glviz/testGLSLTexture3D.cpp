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

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <Eigen/Core>
#include <map>

#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "GLSL.h"
#include "Arcball.h"
#include "GLText.h"
#include "Grid.h"
#include "Circle3D.h"

using namespace std;
using namespace Eigen;
int width=1280, height=1024;
double eyeDistance=-5;
double FOV = 45;
double frame=0.0;
double parabolaQuadraticTerm=0.5;
std::string imageNameFile;
Arcball arcball;

glsl::glShaderManager *SM;
glsl::glShader *shader;
GLuint texName;

int iWidth=512;
int iHeight=512;
int iDepth=512;

//Makes the image into a texture, and returns the id of the texture
// http://pinyotae.blogspot.it/2009/06/note-on-boost-multidimensional-array.html

vector<GLubyte> texture3DVolume;
GLubyte* generateSpheres3D(int width, int height, int depth, int nspheres, int minRadius, int maxRadius, bool addNoise, double noiseDensity, bool usePolkaDots)
{
    if (maxRadius > depth || maxRadius > width  || maxRadius > height)
    {
        throw std::runtime_error("Error can't instance spheres bigger than volume");
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Set the array with zeros
    if ( usePolkaDots )
    {
        // This is a bogus algorithm to avoid intersecting spheres, it can be done better by means
        // of intersection graph and maximal independent set but it need a lot of more code!!
        std::map<int,Circle3D<int> > spheres;
        int radius = mathcommon::unifRand(minRadius,maxRadius);
        spheres[0]=Circle3D<int>( mathcommon::unifRand(radius,width-radius), mathcommon::unifRand(radius,height-radius), mathcommon::unifRand(radius,depth-radius),radius);

        int noncoll=0;
        while (noncoll < nspheres)
        {
            int radius = mathcommon::unifRand(minRadius,maxRadius);
            int centerx = mathcommon::unifRand(radius,width-radius);
            int centery = mathcommon::unifRand(radius,height-radius);
            int centerz = mathcommon::unifRand(radius,depth-radius);
           Circle3D<int> tmp(centerx,centery,centerz,radius);
            bool tmpIsCollidingWithSomeOtherElement=false;
            for (map<int, Circle3D<int> >::iterator iter = spheres.begin(); iter!=spheres.end(); ++iter)
            {
                if  ( iter->second.checkCollision(tmp,5) )
                {
                    tmpIsCollidingWithSomeOtherElement=true;
                    break;
                }
            }
            if ( !tmpIsCollidingWithSomeOtherElement )
                spheres[noncoll++]=tmp;
        }

        for (unsigned int i=0; i<spheres.size(); i++)
        {
            int radius= spheres[i].radius;
            int centerx = spheres[i].centerx;
            int centery = spheres[i].centery;
            int centerz = spheres[i].centerz;

            for (int z=- radius+ centerz; z<=radius+centerz; z++)
            {
				int tYtXx = height*width*z;
                for (int y=-radius+centery; y<=radius+centery; y++)
                {
					int tXY = width*y;
                    for (int x=-radius+centerx; x<=radius+centerx; x++)
                    {
                        if ( SQR((z-centerz)) + SQR((y-centery)) + SQR((x-centerx))  < SQR(radius) )
                        {
                            texture3DVolume[tYtXx*tXY+x]=255;
                        }
                    }
                }
            }
        }
    }
    return texture3DVolume.data();
}

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {  case 'q': //Escape key
    {
        delete shader;
        exit(0);
        break;
    }
    case '+':
    {
        eyeDistance+=0.5;
        break;
    }
    case '-':
    {
        eyeDistance-=0.5;
        break;
    }
    case 'a':
    {
        parabolaQuadraticTerm+=0.05;
        break;
    }
    case 'z':
    {
        parabolaQuadraticTerm-=0.05;
        break;
    }
    }
}

void initRendering()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    
    glEnable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0,0.0,0.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,eyeDistance);

    //makeImage();
    int nspheres=500;
    int minRadius=5;
    int maxRadius=150;
    GLubyte *M = generateSpheres3D(iWidth,iHeight,iDepth,nspheres,minRadius,maxRadius,0,0,1);
    glewInit();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_3D, texName);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    // Important to enable bilinear filtering and smoothing
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glEnable(GL_TEXTURE_3D);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, iWidth, iHeight, iDepth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, M);
#ifdef __linux__
	gluBuild3DMipmaps(texName,GL_LUMINANCE,iWidth,iHeight,iDepth, GL_LUMINANCE, GL_UNSIGNED_BYTE, M);
#endif
	//glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, iWidth, iHeight, iDepth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, M);
    getGLerrors();
}

void drawInfo()
{
    GLText text(width,height,glWhite);
    text.enterTextInputMode();
    text.draw("c= "+util::stringify<double>(parabolaQuadraticTerm));
    text.leaveTextInputMode();
}

void handleResize(int w, int h)
{
    arcball.setWidthHeight(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.1, 200.0);
}

void drawThings()
{
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,eyeDistance);
    arcball.applyRotationMatrix();

    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, texName);
    shader->begin();
    shader->setUniform1f("c",parabolaQuadraticTerm);
    GLUquadric *quad = gluNewQuadric();
    gluQuadricDrawStyle(quad,GLU_FILL);
    gluDisk(quad,0.0,0.5,100,100);
    gluDeleteQuadric(quad);
    /*
    Grid grid;
    grid.init(-0.5,0.5,-0.5,0.5);
    grid.setRowsAndCols(iWidth,iDepth);
    grid.draw(true);
*/
    shader->end();
    glDisable(GL_TEXTURE_3D);
    glPopMatrix();

    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,eyeDistance+0.5);
    arcball.applyRotationMatrix();
    glutWireCube(1);
    glPopMatrix();
    getGLerrors();
}

void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    frame+=2.5;
    arcball.applyRotationMatrix();
    glTranslatef(0.0f, 0.0f, eyeDistance);
    drawThings();
    drawInfo();
    glutSwapBuffers();
}


void mouseFunc(int state, int button, int _x , int _y)
{  if ( button == GLUT_LEFT_BUTTON )
        arcball.startRotation(_x,_y);
    else
        arcball.stopRotation();

    glutPostRedisplay();
}

void mouseDrag(int _x, int _y)
{
    arcball.updateRotation(_x,_y);
    glutPostRedisplay();
}


//Called every 15 milliseconds
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(15, update, 0);
}

int main(int argc, char* argv[])
{
    if (argc<1)
    {
        cerr << "Usage: ./testGLSL3 shadername texture.bmp" << endl;
        cerr << "will load shadername.vert and shadername.frag" << endl;
        exit(0);
    }


    string shaderfile(argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE );
    glutInitWindowSize(width, height);

    glutCreateWindow("test GLSL Volumetric rendering");
    initRendering();

    // Load (and compile, link) from file
    string shadervert = shaderfile+".vert";
    string shaderfrag = shaderfile+".frag";

    SM = new glsl::glShaderManager();
    shader = SM->loadfromFile((char*)shadervert.c_str(), (char*)shaderfrag.c_str());
    delete SM;

    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);

    glutMainLoop();
    return 0;
}
