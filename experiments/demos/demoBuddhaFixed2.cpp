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
// Monitor tilt in mezzo
// Monitor zoom -23

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>
#include <sstream>
#include <vector>
#include <string>
#include <Eigen/Core>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#endif
#ifdef __linux__
#include <GL/glut.h>
#endif

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <gl\gl.h>            // Header File For The OpenGL32 Library
#include <gl\glu.h>            // Header File For The GLu32 Library
#include "glut.h"            // Header File For The GLu32 Library
#endif

/************ INCLUDE CNCSVISION LIBRARY HEADERS ****************/

#include "Mathcommon.h"
#include "Util.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "CoordinatesExtractor.h"
#include "ObjLoader.h"
#include "LatestCalibration.h"

/********* NAMESPACE DIRECTIVES ************************/
using namespace std;
using namespace mathcommon;
using namespace util;
using namespace Eigen;

/********* VARIABLES OBJECTS  **********************/
VRCamera cam;
CoordinatesExtractor headEyeCoords;
ObjLoader model;
static const double focalPlane= -418.5;

/********** EYES AND MARKERS **********************/
Vector3d eyeLeft(0,0,0);
Vector3d eyeRight(0,0,0);
Vector3d probeEnd(0,0,0);
vector <Vector3d> markers;
double interoculardistance=65;
/********* REAL SCREEN POINTS 28/Febbraio/2011 ****/
Screen screen;

/********* VISUALIZATION VARIABLES *****************/
bool gameMode=true;
bool stereo=true;
double modelZ=focalPlane;

static const int StillBuddha=0,RotatingBuddha=1,ExpandingBuddha=2,MovingBuddha=3;
int buddhaMode=StillBuddha;

double frame=0;
/******* FUNCTIONS ***********************************/
void drawModel()
{
    switch (buddhaMode)
    {
    case StillBuddha:
    {
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,modelZ);
        glScalef(800,800,800);
        model.draw();
        glPopMatrix();
    }
    break;
    case RotatingBuddha:
    {
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,modelZ);
        glScalef(800,800,800);
        glRotated(frame,0,1,0);
        model.draw();
        glPopMatrix();
    }
    break;
    case ExpandingBuddha:
    {
        double scaleFactor=sin(frame);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,modelZ);
        glScalef(800,800,800);
        glScalef(scaleFactor,scaleFactor,scaleFactor);
        model.draw();
        glPopMatrix();
    }
    break;
    case MovingBuddha:
    {
        double movefactor=sin(frame);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,modelZ+30*movefactor);
        glScalef(800,800,800);
        model.draw();
        glPopMatrix();
    }
    }

}

void initRendering()
{

    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    BLUEREDLIGHTS
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void drawGLScene()
{   if (stereo)
    {   glDrawBuffer(GL_BACK);
        // Draw left eye view
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cam.setEye(eyeRight);
        drawModel();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cam.setEye(eyeLeft);
        drawModel();
        glutSwapBuffers();
    }
    else
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(eyeRight);
        drawModel();
        glutSwapBuffers();
    }
}

void handleKeypress(unsigned char key, int x, int y)
{   switch (key)
    {   //Quit program
    case 'q':
    case 27:
    {
        exit(0);
    }
    break;

    case ' ':
    {
        buddhaMode++;
        buddhaMode%=4;
    }
    break;
    case 'o':
    {
        static bool orthographic=false;
        cam.setOrthoGraphicProjection(orthographic);
        orthographic=!orthographic;
    }
    break;
    }
}

void handleResize(int w, int h)
{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void update(int value)
{
    eyeLeft  = Vector3d(-interoculardistance/2-30,0,0);
    eyeRight = Vector3d(interoculardistance/2-30,0,0);

    frame+=1.0/(10*TIMER_MS);
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}



int main(int argc, char*argv[])
{
    screen.setWidthHeight(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH);
    screen.setOffset(alignmentX,alignmentY);
    screen.setFocalDistance(focalPlane);
    cam.init(screen);


    glutInit(&argc, argv);
    if (stereo)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
    else
        glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    if ( gameMode )
    {   glutGameModeString("1024x768:32@100");
        if ( glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
            glutEnterGameMode();
        else
        {   cerr << "This resolution is not available\nPress ENTER to continue" << flush;
            cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
            return -1;
        }
    }
    else
    {   glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        glutCreateWindow("CNCSVISION Example 2 HappyBuddha");
        glutFullScreen();
    }
    initRendering();

    model.load("C:/datafiles/objmodels/happyBuddha.obj");
    glutDisplayFunc(drawGLScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutTimerFunc(TIMER_MS, update, 0);
    glutSetCursor(GLUT_CURSOR_NONE);
    /* Application main loop */
    glutMainLoop();
    return 0;
}
