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

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif
#ifdef __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#ifdef _WIN32_
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <fstream>

#include "GLLight.h"
#include "Arcball.h"
#include "ObjLoader.h"
#include "GLUtils.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

using namespace std;
using namespace Eigen;

Arcball arcball;

int width=1024, height=768;
double eyeZ=0;
double eyeDistance=-20;
double FOV = 30;
double znear=0.1;
double zfar=100.0;

double frame=0.0;

void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
    case 'q': //Escape key
        exit(0);
        break;
    }
}

void enableAntialiasing()
{
    glShadeModel(GL_SMOOTH);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);

    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

    // Enable backface culling
    /*
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT_AND_BACK);
    */

}

void initRendering()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    enableAntialiasing();

    //glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDisable(GL_COLOR_MATERIAL);
//    glEnable(GL_NORMALIZE);

    glClearColor(0.0,0.0,0.0,1.0);
    glPointSize(1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,eyeDistance);
}

void handleResize(int w, int h) {

    arcball.setWidthHeight(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, znear, zfar);
}

void drawWireframeSphere()
{
    // This draws a black sphere inside a wireframe white sphere
    glColor3d(1,0,0);
    glutSolidSphere(1,10,20);
    glColor3d(1,1,1);
    glutWireSphere(1,10,20);

    /*
    glPolygonOffset(-1.0f,-1.0f);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glutWireSphere(1,10,20);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
*/
}

void drawHelicoid()
{
    double r=1.0;
    double dPhi=2;
    double dTheta=2;
    double twists=1.0;
    for (double phi = 0; phi<360;phi+=dPhi)
    {
        for (double theta= 0; theta<360*twists;theta+=dTheta)
        {
            double v= phi/180.0*M_PI;
            double u= theta/180.0*M_PI;

            glBegin(GL_QUADS);
            glVertex3d(cos(theta)*(2.0+cos(v))*r,sin(u)*(2.0+cos(v))*r,((u-(2.0*M_PI)) + sin(v))*r);
            v= phi/180.0*M_PI;
            u= (theta+20)/180.0*M_PI;
            glVertex3d(cos(u)*(2.0+cos(v))*r,sin(u)*(2.0+cos(v))*r,((u-(2.0*M_PI)) + sin(v))*r);
            v= (phi+20)/180.0*M_PI;
            u= (theta+20)/180.0*M_PI;
            glVertex3d(cos(u)*(2.0+cos(v))*r,sin(u)*(2.0+cos(v))*r,((u-(2.0*M_PI)) + sin(v))*r);
            v= (phi+20)/180.0*M_PI;
            u= (theta)/180.0*M_PI;
            glVertex3d(cos(u)*(2.0+cos(v))*r,sin(u)*(2.0+cos(v))*r,((u-(2.0*M_PI)) + sin(v))*r);
            glEnd();
        }
    }
}

void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslated(0,0,-10+eyeZ);
    arcball.applyRotationMatrix();
    drawHelicoid();
    drawWireframeSphere();
    glPopMatrix();
    glutSwapBuffers();
}

//Called every 25 milliseconds
void update(int value) {

    glutPostRedisplay();
    glutTimerFunc(25, update, 0);
}

int mouseState;
void mouseFunc(int button, int state, int _x , int _y)
{
    if ( button == GLUT_LEFT_BUTTON && state==GLUT_DOWN )
        arcball.startRotation(_x,_y);

    mouseState=state;
    cerr << state << endl;

    if ( button == 3)
        eyeZ+=1;
    if (button==4)
        eyeZ-=1;

    glutPostRedisplay();
}

void mouseDrag(int _x, int _y)
{
    if (mouseState==GLUT_DOWN)
    {
        arcball.updateRotation(_x,_y);
    }
    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(width, height);

    glutCreateWindow("TEST ANTIALIASING");
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

