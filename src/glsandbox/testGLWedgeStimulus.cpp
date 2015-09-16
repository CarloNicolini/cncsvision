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

#include "GLMaterial.h"
#include "GLLight.h"
#include "Arcball.h"
#include "ObjLoader.h"

#include "WedgePointsStimulus.h"
#include "ExtrudedTrapezePointsStimulus.h"
#include "StimulusDrawer.h"

using namespace std;
using namespace Eigen;

Arcball arcball;
ObjLoader model;
VRCamera cam;
double focalDistance=-100;
double eyeZ=0.0;
double angle=5;
int width=800, height=800;
StimulusDrawer stimDrawer;
StimulusDrawer stimDrawerPlanes[2];

void initVariables();
void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q': //Escape key
    {
        exit(0);
        break;
    }
    case '+':
    {
        eyeZ+=1;
        break;
    }
    case '-':
    {
        eyeZ-=1;
        break;
    }
    case 'a':
    {
        angle+=5.0;
        initVariables();
        break;
    }
    case 's':
    {
        angle-=5.0;
        initVariables();
        break;
    }
    case 13:
    {
        initVariables();
        break;
    }
    }
}

void initRendering()
{
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
    cam.init(Screen(20,20,0,0,focalDistance));
    cam.setNearFarPlanes(1.0,200.0);
    cam.setOrthoGraphicProjection(true);
}

void initVariables()
{
    double L = 2.5;
    double alfa = angle*M_PI/180.0;
    /*
    BoxNoiseStimulus planes[2];
    double density = 0.0125;
    for (int i=0; i<2; i++)
    {
        int NPoints = L*L*density;
        planes[i].setDimensions(L,0.01,L);
        planes[i].setNpoints(1000);
        planes[i].compute();
        stimDrawerPlanes[i].setSpheres(false);
        stimDrawerPlanes[i].setStimulus(&planes[i]);
        stimDrawerPlanes[i].initList(&planes[i],glRed);
    }
    */

    WedgePointsStimulus wedge;
    wedge.setNpoints(500);
    wedge.setAngle(alfa);
    wedge.setPlanesSize(2*L, L);
    wedge.compute();
    stimDrawer.setSpheres(false);
    stimDrawer.setStimulus(&wedge);
    stimDrawer.initList(&wedge,glRed);
    cerr << "angle= " << angle << endl;
}

void handleResize(int w, int h)
{
    arcball.setWidthHeight(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void drawThings()
{
    glDisable(GL_LIGHTING);
    double a = angle*M_PI/180.0;
    double L = 2.5;

    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    arcball.applyRotationMatrix();
    glBegin(GL_LINES);
    glVertex3d(-8,0,0);
    glVertex3d(8,0,0);
    glEnd();
    //glutWireSphere(1,20,20);
    glPopMatrix();

    /*
    glPushMatrix();
    glTranslated(0,0,-L/2);
    glRotated(angle/2,1,0,0);
    //glScaled(1,1.0/sin(0.5*a),1);
    stimDrawerPlanes[0].draw();
    glPopMatrix();
    */

    /*
    int n=0;
    double h = 1.0;
    double salfa = sin(a);
    double calfa = cos(a);
    glBegin(GL_POINTS);
    while (n<1000)
    {
        double x = mathcommon::unifRand(-L/2,L/2);
        double y = mathcommon::unifRand(-h/2,h/2);
        double z = mathcommon::unifRand(-L/2,L/2);
        if ( y<= z*salfa+1E-2 && y>= z*salfa-1E-2)
        {
            n++;
            glVertex3d(x,y,z);
        }
    }
    glEnd();
    */
    /*
    glPushMatrix();
    glRotated(-angle/2,1,0,0);
    glScaled(1,1/sin(angle*M_PI/180.0),1);
    stimDrawerPlanes[1].draw();
    glPopMatrix();
    */
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,focalDistance);
    arcball.applyRotationMatrix();
    stimDrawer.draw();
    glPopMatrix();
}

void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cam.setEye(Vector3d(0.0,0.0,0.0));
    drawThings();
    glutSwapBuffers();
}

void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(15, update, 0);
}

void mouseFunc(int state, int button, int _x , int _y)
{
    if ( button == GLUT_LEFT_BUTTON )
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


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);

    glutCreateWindow("WedgeStimulus");
    initRendering();
    initVariables();

    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);

    glutMainLoop();
    return 0;
}

