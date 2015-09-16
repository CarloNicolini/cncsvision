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
#include "GLTexture.h"
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

GLUquadricObj* myReusableQuadric;
GLTexture tex;

void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
    case 'q': //Escape key
        exit(0);
        break;
    case ' ':
        tex.loadBMP("../data/textures/envmap.bmp");
        break;
    }
}

void initRendering() {
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
    glTranslatef(0,0,eyeDistance);

}

void handleResize(int w, int h)
{
    arcball.setWidthHeight(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.1, 200.0);
}

void drawReference()
{

    double length=4;
    glPushMatrix();
    //glTranslatef(length/2,0,0);
    glPushMatrix();
    //glColor3f(0,0,1);
    gluCylinder( myReusableQuadric, 0.1, 0.1, length, 50, 50 );
    glTranslatef(0,0,length);
    gluCylinder( myReusableQuadric,0.3, 0.0, 1, 50,50 );
    glPopMatrix();

    glPushMatrix();
    glRotated(-90,1,0,0);
    //glColor3f(0,1,0);
    gluCylinder( myReusableQuadric, 0.1, 0.1, length, 50, 50 );
    glTranslatef(0,0,length);
    gluCylinder( myReusableQuadric,0.3, 0.0, 1, 50,50 );
    glPopMatrix();

    glPushMatrix();
    glRotated(90,0,1,0);
    //glColor3f(1,0,0);
    gluCylinder( myReusableQuadric, 0.1, 0.1, length, 50, 50 );
    glTranslatef(0,0,length);
    gluCylinder( myReusableQuadric,0.3, 0.0, 1, 50,50 );
    glPopMatrix();
    glPopMatrix();
}

void drawImage()
{
    /*
    glBegin(GL_QUADS);
    glTexCoord2d(0,0);
    glVertex3d(-1,-1,0);

    glTexCoord2d(1,0);
    glVertex3d(1,-1,0);

    glTexCoord2d(1,1);
    glVertex3d(1,1,0);

    glTexCoord2d(0,1);
    glVertex3d(-1,1,0);
    glEnd();
    tex.use();
    */
    tex.drawFrame(5,5,0);
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, eyeZ+eyeDistance);
    arcball.applyRotationMatrix();
    //drawReference();
    drawImage();
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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Carloball");
    initRendering();
    myReusableQuadric =  gluNewQuadric();
    // Texture must be loaded after the glut initialization!
    tex.loadBMP("../data/textures/envmap.bmp");
    //tex.loadBMP("C:/cncsvisioncmake/data/textures/envmap.bmp");
    //tex.loadBMP("data/textures/cubemapping/cubemap_grid/cubemap_grid.bmp");
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);

    glutMainLoop();
    return 0;
}

