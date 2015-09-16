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
#include "IncludeGL.h"

#include "Util.h"
#include "Arcball.h"
#include "GLUtils.h"
#include "ObjLoader2.h"
#include "TinyObjLoader.h"
#include "GLSL.h"

using namespace std;
using namespace Eigen;

Arcball arcball;
ObjLoader2 obj;
glsl::glShaderManager *SM;
glsl::glShader *shader;

int width=1024, height=768;
double eyeZ=0;
double eyeDistance=-20;
double FOV = 30;
double znear=0.1;
double zfar=1000.0;
double frame=0.0;
/**
 * @brief handleKeypress
 * @param key
 * @param x
 * @param y
 */
void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q': //Escape key
    {
        exit(0);
        break;
    }
    case ' ':
    {
        break;
    }
    case '+':
        eyeZ+=0.2;
        break;
    case '-':
        eyeZ-=0.2;
        break;
    }
}

/**
 * @brief initRendering
 */
void initRendering()
{
    glEnable(GL_DEPTH_TEST);
    /*
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    //BLUEREDLIGHTS;
    */
    glClearColor(0.0,0.0,0.0,1.0);
    glPointSize(1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,eyeDistance);
    /*
    SM = new glsl::glShaderManager();

    const GLcharARB vertexShader[] = STATIC_STRINGIFY(
    void main()
    {
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    }
    );

    const GLcharARB fragmentShader[] =
            STATIC_STRINGIFY(
                void main()
    {
                    gl_FragColor = vec4(1.0);
                }
                );

    shader = SM->loadfromMemory(vertexShader,fragmentShader);
    delete SM;
*/
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
    frame+=0.1;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLineWidth(0.01);
    glLoadIdentity();
    glPushMatrix();
    glTranslated(0,0,-10+eyeZ);
    arcball.applyRotationMatrix();
    //obj.setFillColor(x);
    //obj.setLineColor(l);
    //    shader->begin();

    // Draw polygon in front here (or wireframe)
    // Move every polygon rendered backward a bit before depth testing
    obj.setFillColor(1.0,1.0,1.0,1.0);
    obj.draw( GL_LINE ); //draw the object with full black faces
    glPolygonOffset(1.0f, 1.0f);
    obj.setFillColor(0.0,0.0,0.0,1.0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    obj.draw( GL_FILL ); //draw the object with full black faces
    glDisable(GL_POLYGON_OFFSET_FILL);

    //    shader->end();
    glutWireCube(1.0);
    glPopMatrix();
    glutSwapBuffers();
}

/**
 * @brief update
 * @param value
 */
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(25, update, 0);
}

/**
 * @brief mouseFunc
 * @param state
 * @param button
 * @param _x
 * @param _y
 */
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
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(width, height);

    glutCreateWindow("OBJ Loader");
    initRendering();
    glewExperimental=0;
    glewInit();
    //obj.load("/home/carlo/workspace/cncsvisioncmake/data/objmodels/happyBuddha.obj");
    obj.load("../data/objmodels/helicoid.obj");

    //obj.load("/home/carlo/Desktop/obj/buddha.obj");
    //obj.center();
    obj.normalizeToUnitBoundingBox();
    obj.initializeBuffers();
    obj.getInfo();
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);
    glutMainLoop();
    return 0;
}

