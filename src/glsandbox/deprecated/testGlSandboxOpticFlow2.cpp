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

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "Timer.h"
#include "GLMaterial.h"
#include "GLLight.h"
#include "Arcball.h"
#include "ObjLoader.h"
#include "OpticFlowEstimator.h"

using namespace std;
using namespace Eigen;

Arcball arcball;
ObjLoader model;
OpticFlowEstimator flow;
static const int width=450, height=450;
double eyeZ=0;
double eyeDistance=-20;
double FOV = 30;
double alpha=1.3;
double frame=0.0;
bool opticflow=true;

ofstream frame1,frame2;
int win1,win2;
void getBuffer()
{
    Matrix<float> rgbabuffer(width,height);

    glReadPixels(0,0,width,height,GL_LUMINANCE, GL_FLOAT, rgbabuffer.data());

    ofstream rgbabufferout;
    rgbabufferout.open("rgba.dat");
    rgbabufferout << rgbabuffer << endl;
    rgbabufferout.close();
}

void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
    case 'q': //Escape key
        exit(0);
        break;

    case 'w':
        getBuffer();
        break;

    case 'o':
        opticflow=!opticflow;
        break;
    case '+':
        alpha+=0.1;
        cerr << alpha << endl;
        break;
    case '-':
        alpha-=0.1;
        cerr << alpha << endl;
        break;
    }
}

void initRendering() {
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.0,0.0,0.0,1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void initVariables()
{
}

void handleResize(int w, int h) {
    
    arcball.setWidthHeight(w, h);
    flow.init(w,h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.1, 200.0);
}



void drawThings()
{
    glPushMatrix();

    glLoadIdentity();
    // This is a light that is attached to the teapot

    glDisable(GL_BLEND);
    glTranslated(0,0,-5);
    arcball.applyRotationMatrix();
    glRotated(5*frame,0,1,0);
    glutSolidTeapot(0.5);
    glPopMatrix();

    glPushMatrix();
    glLoadIdentity();
    glTranslated(3*cos(frame),2.0*sin(frame/10),eyeDistance+eyeZ);
    arcball.applyRotationMatrix();

    //glRotated(10*frame,0,1,0);
    glutWireSphere(0.8,10,20);
    glPopMatrix();
}

void drawScene()
{    
    glutSetWindow(win1);
    flow.getPreBuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, eyeZ+eyeDistance);
    drawThings();
    flow.getPostBuffer();
    glutSwapBuffers();
}

void drawScene2()
{    
    glutSetWindow(win2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5,0,0,1);
    if ( opticflow )
    {
        flow.computeHS(alpha);
        flow.draw();
    }
    glutSwapBuffers();
}

//Called every 15 milliseconds
void update(int value) {

    frame+=0.05;

    glutPostRedisplay();
    glutTimerFunc(5, update, 0);
}

void mouseFunc(int state, int button, int _x , int _y)
{
    if ( button == GLUT_LEFT_BUTTON )
        arcball.startRotation(_x,_y);
    else
        arcball.stopRotation();

    if ( state == 3 )
    {   //eyeZ+=0.1;
        alpha+=0.01;
        cerr << alpha << endl;
    }
    if (state ==4)
    {   //eyeZ-=0.1;
        alpha-=0.01;
        cerr << alpha << endl;
    }
    glutPostRedisplay();

}

void mouseDrag(int _x, int _y)
{  

    arcball.updateRotation(_x,_y);
    glutPostRedisplay();
}

void initCommonCallbacks()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
}

void drawSceneAll()
{
    drawScene();
    drawScene2();
}



int main(int argc, char** argv)
{
    
    // GLUT INIT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);

    win1 = glutCreateWindow("OpticFlow2.0");

    initRendering();
    initVariables();

    glutDisplayFunc(drawSceneAll);
    initCommonCallbacks();
    
    win2 = glutCreateSubWindow(win1, width/2 , 0, width/2, height);
    glutDisplayFunc(drawScene2);
    initCommonCallbacks();
    
    glutTimerFunc(15, update, 0);
    glutMainLoop();
    return 0;
}

