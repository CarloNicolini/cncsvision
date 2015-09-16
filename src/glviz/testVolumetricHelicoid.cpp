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
#include <map>
#include <Eigen/Core>

#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "GLSL.h"
#include "Arcball.h"
#include "GLText.h"
#include "Circle3D.h"
#include "VolumetricMeshIntersection.h"

#include "VRCamera.h"
#include "Screen.h"

using namespace std;
using namespace Eigen;
int width=1280, height=1280;
double eyeDistance=-500;
double FOV = 45;
double frame=0.0;
std::string imageNameFile;
Arcball arcball;
GLuint texName;

int iWidth=768;
int iHeight=768;
int iDepth=768;
VolumetricMeshIntersection surface;
/**
 * @brief handleKeypress
 * @param key
 * @param x
 * @param y
 */
Eigen::Vector3d offset(0.0,0.0,0.0);
void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q': //Escape key
    {
        exit(0);
        break;
    }
    case '7':
    {
        surface.meshStruct.offsetZ+=2.0;
        break;
    }
    case '9':
    {
        surface.meshStruct.offsetZ-=2.0;
        break;
    }
    case '2':
    {
        surface.meshStruct.offsetY-=2.0;
        break;
    }
    case '8':
    {
        surface.meshStruct.offsetY+=2.0;
        break;
    }
    case '4':
    {
        surface.meshStruct.offsetX-=2.0;
        break;
    }
    case '6':
    {
        surface.meshStruct.offsetX+=2.0;
        break;
    }
    case '+':
    {
        eyeDistance+=5;
        break;
    }
    case '-':
    {
        eyeDistance-=5;
        break;
    }
    case 'h':
    {
        surface.meshStruct.height+=0.05;
        break;
    }
    case 'j':
    {
        surface.meshStruct.height-=0.05;
        break;
    }
    case 'w':
    {
        surface.meshStruct.radius+=2;
        break;
    }
    case 'e':
    {
        surface.meshStruct.radius-=2;
        break;
    }
    case 'a':
    {
       surface.meshStruct.rotationAngle+=2*M_PI/96;

        break;
    }
    case 'z':
    {
        surface.meshStruct.rotationAngle-=2*M_PI/96;
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

    int nspheres=500;
    int minRadius=15;
    int maxRadius=100;
    glewInit();
    surface.resize(iWidth,iHeight,iDepth);
    surface.loadObj("../data/objmodels/helicoid.obj");
    surface.setUniformColor(glWhite);
    surface.fillVolumeWithSpheres(nspheres,minRadius,maxRadius);
    surface.initializeTexture();

    surface.initializeSurfaceShaders();

    surface.meshStruct.radius=120.0;
    surface.meshStruct.height=1.0;
    surface.meshStruct.rotationAngle=0.0;
    surface.meshStruct.offsetX=0.0;
    surface.meshStruct.offsetY=0.0;
    surface.meshStruct.offsetZ=0.0;

    getGLerrors();
}

void handleResize(int w, int h)
{
    arcball.setWidthHeight(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 1.0, 1000.0);
}

void drawThings()
{
    GLText text(width,height,glWhite);
    text.init(width,height,glWhite);
    text.enterTextInputMode();

    text.draw("objSize= "+util::stringify(surface.meshStruct.radius));
    text.draw("theta= "+util::stringify(surface.meshStruct.rotationAngle));
    text.draw("objCenterX= "+util::stringify(surface.meshStruct.offsetX));
    text.draw("objCenterY= "+util::stringify(surface.meshStruct.offsetY));
    text.draw("objCenterZ= "+util::stringify(surface.meshStruct.offsetZ));

    text.leaveTextInputMode();

    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,eyeDistance);
    arcball.applyRotationMatrix();
    glRotated(90,1,0,0);
    surface.draw();
    glPopMatrix();
/*
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,eyeDistance);
    arcball.applyRotationMatrix();
    glRotated(90,1,0,0);
    glTranslated(surface.meshStruct.offsetX,surface.meshStruct.offsetY,surface.meshStruct.offsetZ);
    glutWireCube(surface.meshStruct.radius*2);
    glPopMatrix();
*/
    getGLerrors();
}

/**
 * @brief drawScene
 */
void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    frame+=0.01;
    arcball.applyRotationMatrix();
    glTranslatef(0.0f, 0.0f, eyeDistance);
    drawThings();
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
        cerr << "Usage: ./testGLSL2 shadername texture.bmp" << endl;
        cerr << "will load shadername.vert and shadername.frag" << endl;
        exit(0);
    }


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE );
    glutInitWindowSize(width, height);

    glutCreateWindow("test GLSL Volumetric rendering");
    initRendering();

    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);

    glutMainLoop();
    return 0;
}

