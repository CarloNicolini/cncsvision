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
#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "Arcball.h"
#include "GLText.h"

#include "VolumetricSurfaceIntersection.h"

using namespace std;
using namespace Eigen;
int width=640, height=640;
double eyeDistance=-25;
double FOV = 45;
double frame=0.0;
std::string imageNameFile;
Arcball arcball;
GLuint texName;

int iWidth=512;
int iHeight=512;
int iDepth=512;
VolumetricSurfaceIntersection surface;

/**
 * @brief handleKeypress
 * @param key
 * @param x
 * @param y
 */
void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {  case 'q': //Escape key
    {
        exit(0);
        break;
    }
    case ' ':
    {
        surface.fillVolumeWithSpheres(1500,5,10);
        surface.initializeTexture();
        break;
    }
    case '+':
    {
        eyeDistance+=0.5;
        break;
    }
    case '-':
    {        eyeDistance-=0.5;
        break;
    }
    case 'a':
    {
        surface.parabolaSurface.curvature+=0.01;
        surface.parabolicCylinderSurface.curvature+=0.01;
        break;
    }
    case 'z':
    {
        surface.parabolaSurface.curvature-=0.01;
        surface.parabolicCylinderSurface.curvature-=0.01;
        break;
    }
    }
}

/**
 * @brief initRendering
 */
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
    int maxRadius=50;
    glewInit();
    surface.resize(iWidth,iHeight,iDepth*3);
    surface.fillVolumeWithSpheres(nspheres,minRadius,maxRadius);
    surface.initializeTexture();
    surface.initializeSurfaceShaders(VolumetricSurfaceIntersection::SurfaceParabolicCylinder);

    surface.parabolicCylinderSurface.curvature=1.0;
    surface.parabolicCylinderSurface.centerX=surface.parabolicCylinderSurface.centerY=surface.parabolicCylinderSurface.centerZ=0.0;

    surface.parabolaSurface.curvature=1.0;
    surface.coneSurface.c=1.0;
    surface.ellipsoidSurface.axisX=1.0;
    surface.ellipsoidSurface.axisY=1.0;
    surface.ellipsoidSurface.axisZ=1.0;
    surface.ellipticCylinderSurface.axisX=1.0;
    surface.ellipticCylinderSurface.height=1.0;
    surface.ellipticCylinderSurface.axisZ=1.0;
    getGLerrors();
}

/**
 * @brief handleResize
 * @param w
 * @param h
 */
void handleResize(int w, int h)
{
    arcball.setWidthHeight(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.1, 200.0);
}

/**
 * @brief drawThings
 */
void drawThings()
{
    GLText text(width,height,glWhite);
    text.init(width,height,glWhite);
    text.enterTextInputMode();
    text.draw("C= "+util::stringify(surface.parabolaSurface.curvature));
    text.leaveTextInputMode();
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,eyeDistance);
    glScaled(10,10,10);
    arcball.applyRotationMatrix();
    surface.draw();
    glPopMatrix();
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

/**
 * @brief mouseFunc
 * @param state
 * @param button
 * @param _x
 * @param _y
 */
void mouseFunc(int state, int button, int _x , int _y)
{  if ( button == GLUT_LEFT_BUTTON )
        arcball.startRotation(_x,_y);
    else
        arcball.stopRotation();

    glutPostRedisplay();
}

/**
 * @brief mouseDrag
 * @param _x
 * @param _y
 */
void mouseDrag(int _x, int _y)
{
    arcball.updateRotation(_x,_y);
    glutPostRedisplay();
}

/**
 * @brief update
 * @param value
 */
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(15, update, 0);
}

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
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
