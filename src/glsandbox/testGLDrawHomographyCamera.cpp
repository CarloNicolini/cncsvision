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
//#include "CameraSimpleModel.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

using namespace std;
using namespace Eigen;

Arcball arcball;

int width=800, height=800;
double eyeZ=-800.0;
double FOV = 30.0;
double znear=10.0;
double zfar=5000.0;
double frame=0.0;
int mouseState=0;

std::string points2Dfilename("/home/carlo/Desktop/3D-Display/cameraCalibration/CALIBRATIONS/06_05_2013/2D_points.txt");
std::string points3Dfilename("/home/carlo/Desktop/3D-Display/cameraCalibration/CALIBRATIONS/06_05_2013/3D_points.txt");

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key) {
    case 'q': //Escape key
        exit(0);
        break;
    }
}

/**
 * @brief enableAntialiasing
 */
void enableAntialiasing()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
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

/**
 * @brief initRendering
 */
void initRendering()
{
    enableAntialiasing();
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat pos[]={1.0f,1.0f,1.0f};
    GLfloat diffuse[]={0.5f,0.5f,0.5f};
    GLfloat specular[]={1.0f,1.0f,1.0f};
    GLfloat ambient[]={0.3f,0.3f,0.3f};

    glLightfv(GL_LIGHT0,GL_POSITION,pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse );
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular );
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient  );
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(0.0,0.0,0.0,1.0);
    glPointSize(1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,eyeZ);
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
    gluPerspective(FOV, (float)w / (float)h, znear, zfar);
}
/**
 * @brief drawScene
 */
void drawScene()
{
    double calibrationCameraZNear = 0.5;
    double calibrationCameraZFar = 400.0;
    CameraDirectLinearTransformation cam(points2Dfilename,points3Dfilename,true,true,0,0,604,684,calibrationCameraZNear,calibrationCameraZFar);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    double theta = acos( Vector3d::UnitZ().dot(cam.getPrincipalAxis())/cam.getPrincipalAxis().norm() );
    Eigen::Affine3d cameraOrientation = Eigen::Affine3d::Identity();
    cameraOrientation.linear() = Eigen::AngleAxisd( theta, cam.getPrincipalAxis() ).toRotationMatrix();
    static bool printed=false;
    if (!printed)
    {
        cout << "HZ 3x4 projection matrix=\n" << cam.getProjectionMatrix() << endl;
        cout << "Intrinsinc camera matrix=\n" <<cam.getIntrinsicMatrix() << endl;
        cout << "Extrinsic camera matrix=\n"<< cam.getRotationMatrix() << endl << endl;
        cout << "Camera Center C=" << cam.getCameraPositionWorld().transpose() << endl;
        cout << "Camera t= " << cam.getT().transpose() << endl;
        cout << "Camera Principal axis= " << cam.getPrincipalAxis().transpose() << endl;
        cout << "Camera Principal point=" << cam.getPrincipalPoint().transpose() << endl ;
        cout << "OpenGL ModelViewMatrix=\n" << cam.getOpenGLModelViewMatrix().matrix() << endl;
        cout << "OpenGL Projection=\n" << cam.getOpenGLProjectionMatrix().matrix() << endl;
        printed=true;
    }

    glPushMatrix();
    glLoadIdentity();
    glTranslated(0.0,0.0,eyeZ);
    arcball.applyRotationMatrix();
    glColor3d(1.0,1.0,1.0);
    drawFrustum(cam.getOpenGLProjectionMatrix().data(),&cam.getOpenGLModelViewMatrix().inverse().matrix()(0,0));
    glPopMatrix();

    // Draw the world reference frame
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,eyeZ);
    arcball.applyRotationMatrix();
    glBegin(GL_LINES);
    glColor3d(1,0,0);
    glVertex3d(0,0,0);
    glVertex3d(50,0,0);

    glColor3d(0,1,0);
    glVertex3d(0,0,0);
    glVertex3d(0,50,0);

    glColor3d(0,0,1);
    glVertex3d(0,0,0);
    glVertex3d(0,0,50);
    glEnd();
    glPopMatrix();

    glutSwapBuffers();
}

//Called every 25 milliseconds
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(25, update, 0);
}

void mouseFunc(int button, int state, int _x , int _y)
{
    if ( button == GLUT_LEFT_BUTTON && state==GLUT_DOWN )
        arcball.startRotation(_x,_y);

    mouseState=state;

    if ( button == 3)
        eyeZ+=20;
    if (button==4)
        eyeZ-=20;

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


