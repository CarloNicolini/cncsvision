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
#include <fstream>

#include "GLLight.h"
#include "Arcball.h"
#include "ObjLoader.h"
#include "GLUtils.h"
#include "Homography.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

using namespace std;
using namespace Eigen;

Arcball arcball;
int width=600, height=600;
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
}

void drawScene()
{
    glPushMatrix();
    std::vector<Eigen::Vector4d> M(8);
    M[0] << -1,-1,1,1;
    M[1] << 1,-1,1,1;
    M[2] << -1,1,1,1;
    M[3] << 1,1,1,1;
    M[4] <<  -1,-1,-1,1;
    M[5] <<  1,-1,-1,1;
    M[6] << -1,1,-1,1;
    M[7] <<  1,1,-1,1;

    std::vector<Eigen::Vector3d> m(8);

    Eigen::Projective3d P;
    Eigen::Affine3d MV;
    Eigen::Vector4i VP;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslated(0,0,-10+eyeZ);
    arcball.applyRotationMatrix();
    glGetDoublev(GL_MODELVIEW_MATRIX,MV.data());
    glGetDoublev(GL_PROJECTION_MATRIX,P.data());
    glGetIntegerv(GL_VIEWPORT,VP.data());

    for (int i=0; i<8; i++)
    {
        double px,py,pz;
        gluProject(M[i].x(),M[i].y(),M[i].z(),MV.data(),P.data(),VP.data(),&px,&py,&pz);
        m[i] << px,py,pz;
    }

    // Draw the original points
    glColor3d(1,1,1);
    glBegin(GL_POINTS);
    for (int i=0; i<8; i++)
    {
        glVertex3dv(M[i].data());
    }
    glEnd();
    CameraDirectLinearTransformation cam(m,M,true,true,0,0,width,height,znear,zfar);

    cout << "ORIGINAL GL_PROJECTION=\n" << P.matrix() << endl;
    cout << "ESTIMATED GL_PROJECTION=\n" << cam.getOpenGLProjectionMatrix().matrix() << endl;
    cout << "ORIGINAL GL_MODELVIEW=\n" << MV.matrix() << endl;
    cout << "ESTIMATED GL_MODELVIEW=\n" << cam.getOpenGLModelViewMatrix().matrix() << endl;
    cout << "CAMERA Principal point= " << cam.getPrincipalPoint().transpose() << endl;
    cout << "CAMERA Principal axis= " << cam.getPrincipalAxis().transpose() << endl;
    cout << "Camera center= " << cam.getCameraPositionWorld().transpose() << endl;
    glPopMatrix();

    // Draw computed principal axis
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,-10+eyeZ);
    glBegin(GL_LINES);
    glVertex3d(0,0,0);
    glColor3d(1.0,0.0,0.0);
    glVertex3dv(cam.getPrincipalAxis().data());
    glEnd();
    glPopMatrix();

    // Draw original principal axis
    glPushMatrix();
    glTranslated(0,0,-10+eyeZ);
    arcball.applyRotationMatrix();
    glColor3d(1.0,1.0,0.0);
    glBegin(GL_LINES);
    glVertex3d(0,0,1);
    glVertex3d(0,0,-1);
    glEnd();
    glPopMatrix();

    // Draw the points as if their projection were computed using the CameraDirectLinearTransformation matrices:

    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(cam.getOpenGLProjectionMatrix().data());
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(cam.getOpenGLModelViewMatrix().data());
    glColor3d(1,0,1);
    glBegin(GL_POINTS);
    for (int i=0; i<8; i++)
    {
        glVertex3dv(M[i].data());
    }
    glEnd();

    glPopMatrix();

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


