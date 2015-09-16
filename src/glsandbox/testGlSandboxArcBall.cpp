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
#include "GLMaterial.h"
#include "GLLight.h"
#include "Arcball.h"
#include "ObjLoader.h"
#include "GLUtils.h"

using namespace std;
using namespace Eigen;

Arcball arcball;

int width=1024, height=768;
double eyeZ=0;
double eyeDistance=-20;
double FOV = 30;
double znear=0.1;
double zfar=1000.0;

double frame=0.0;

void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
    case 'q': //Escape key
        exit(0);
        break;
    }
}

void initRendering()
{
    glEnable(GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
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


void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);

    vector<Vector3d> points;
    points.push_back(Vector3d(1,0,0));
    points.push_back(Vector3d(0,1,0));
    points.push_back(Vector3d(0,0,1));
    points.push_back(Vector3d(-1,-1,-1));
    points.push_back(Vector3d(1,1,1));
    points.push_back(Vector3d(0,0,0));
    points.push_back(Vector3d(-0.5,0.5,0.0));
    points.push_back(Vector3d(-0.2,-0.4,0.2));
    points.push_back(Vector3d(-0.4,0.1,0.8));
    points.push_back(Vector3d(0.2,0.51,0.118));
    points.push_back(Vector3d(5,1,2));
    points.push_back(Vector3d(1,4,6));
    points.push_back(Vector3d(10,-1,5));

    Affine3d M;
    Projective3d P;
    Vector4i viewport(0,0,width,height);

    ofstream data2d("/home/carlo/Desktop/cameraCalibration/p2d.txt");
    ofstream data3d("/home/carlo/Desktop/cameraCalibration/p3d.txt");
    ofstream MVPdata("/home/carlo/Desktop/cameraCalibration/MVP.txt");

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, eyeZ+eyeDistance);
    glRotated(30,0,1,1);
    glTranslated(5,0,0);
    glRotated(30,0,1,0);
    glGetDoublev(GL_MODELVIEW_MATRIX,M.data());
    glGetDoublev(GL_PROJECTION_MATRIX,P.data());
    glColor3fv(glGreen);
    glutWireSphere(1.01,10,10);

    glBegin(GL_POINTS);
    for (int i=0; i<points.size(); i++)
    {
        glVertex3dv(points[i].data());
        double wx,wy,wz;
        gluProject(points[i].x(),points[i].y(),points[i].z(),M.data(),P.data(),viewport.data(),&wx,&wy,&wz);
        data3d << points[i].transpose() << endl;
        data2d << wx << " "<< wy << " " << endl;
    }
    glEnd();
    Projective3d MP = M*P;
    Matrix4d MPm = M.matrix()*P.matrix();
    MVPdata << "P=\n" << P.matrix() << "\n\nM=\n" << M.matrix() << "\n\nM*P=\n" << MP.matrix() << "\n\nP*M=\n" << (P*M).matrix() << endl << endl;
    //exit(0);
    glPopMatrix();

    Eigen::Matrix4d PP;
    PP.matrix() <<
                   2.74073  ,      0  ,      0 ,      -0,
            0 , 3.73205  ,      0  ,     -0,
            0 ,       0 , -1.0002, -0.20002,
            0  ,      0   ,    -1 ,      -0;

    glPushMatrix();
    // Swith to projection mode
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(PP.data());
    // Switch to modelview mode
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(M.data());

    glColor3fv(glRed);
    glutWireSphere(1.50,10,10);
    glPopMatrix();


    glutSwapBuffers();
}

//Called every 25 milliseconds
void update(int value) {

    glutPostRedisplay();
    glutTimerFunc(25, update, 0);
}



void mouseFunc(int state, int button, int _x , int _y)
{
    if ( button == GLUT_LEFT_BUTTON )
        arcball.startRotation(_x,_y);
    else
        arcball.stopRotation();

    if ( state == 3 )
        eyeZ+=0.1;
    if (state ==4)
        eyeZ-=0.1;

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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(width, height);

    glutCreateWindow("Carloball");
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

