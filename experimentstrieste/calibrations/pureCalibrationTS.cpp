// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2011 Carlo Nicolini <carlo.nicolini@iit.it>
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

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <math.h>
#include <limits>
#include <sstream>
#include <vector>
#include <string>
#include <deque>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "IncludeGL.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif



/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/
#include "Mathcommon.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "GLText.h"
#include "LatestCalibrationTrieste.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace Eigen;


/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
Vector3d eyeRight,eyeLeft;

static const Vector3d center(0,0,baseFocalDistance);

/********* REAL SCREEN POINTS ****/
Screen screen;
double crossSize=150;	// millimeters
/********** EYES AND MARKERS **********************/
static double interoculardistance=65;

/********* VISUALIZATION VARIABLES *****************/
static const bool gameMode=true;
static const bool stereo=false;

/*************************** FUNCTIONS ***********************************/

void initRendering()
{
    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Set depth buffer clear value */
    glClearDepth(1.0);
    /* Enable depth test */
    glEnable(GL_DEPTH_TEST);
    /* Set depth function */
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawGLScene()
{
    if (stereo)
    {
        glDrawBuffer(GL_BACK);
        // Draw left eye view
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0,1.0,1.0,1.0);
        cam.setEye(eyeRight);

        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0,1.0,1.0,1.0);
        cam.setEye(eyeLeft);
        glutSwapBuffers();
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0,1.0,1.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
        glLineWidth(3);
        glPushMatrix();
        glLoadIdentity();
        // Here we draw the central cross, must be 150 millimeters on horizontal and vertical in order to have a correct monitor calibration
        glColor3fv(glBlack);
        glBegin(GL_LINES);
        glVertex3d(-crossSize/2,0,baseFocalDistance);
        glVertex3d(crossSize/2,0,baseFocalDistance);

        glVertex3d(0,crossSize/2,baseFocalDistance);
        glVertex3d(0,-crossSize/2,baseFocalDistance);
        glEnd();
        glPopMatrix();
        glutSwapBuffers();
    }
}

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {
        exit(0);
    }
    break;
    }
}

void handleResize(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void initProjectionScreen(double _focalDist, const Affine3d &_transformation)
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setFocalDistance(_focalDist);
    screen.transform(_transformation);
    cam.init(screen);
}

void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}


int main(int argc, char*argv[])
{

    // Initializes the optotrak and starts the collection of points in background
    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if (gameMode==false)
    {   glutInitWindowSize( SCREEN_WIDTH , SCREEN_HEIGHT );
        glutCreateWindow("BASE CALIBRATION FOR MONITOR SIZE");
        glutFullScreen();
    }
    else
    {   glutGameModeString(TS_GAME_MODE_STRING);
        glutEnterGameMode();
        glutFullScreen();
    }
    initRendering();
    initProjectionScreen(baseFocalDistance,Affine3d::Identity());

    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();


    return 0;
}
